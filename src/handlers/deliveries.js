import { spawn } from 'child_process';

const a_star_file_path = './dist/a_star';
const knapsack_file_path = './dist/knapsack';

// Must be relative to the program
const graph_file = './src/paths/graph.txt'
const heuristic_file = './src/paths/heuristic.txt'
const city_mapping_file = './src/paths/city_mapping.txt'

function parseRouteString(routeStr) {
    const regex = /^(.+?) -> (.+?) via (.+?) \(([\d.]+) km\)$/;
    const match = routeStr.match(regex);

    if (!match) {
        throw new Error("Invalid route format");
    }

    const [, source, destination, roadName, distanceStr] = match;
    return {
        source,
        destination,
        roadName,
        distance: parseFloat(distanceStr)
    };
}
/*
 * Suggests a delivery route
 * @param source: integer Node representing the source 
 * @param destination: integer Note representing the destination
 */
export const suggestDeliveryRoute = (source, destination) => {
    return new Promise((resolve, reject) => {
        const aStarProcess = spawn(a_star_file_path, [graph_file, heuristic_file, city_mapping_file, source, destination]);
        let output = '';
        aStarProcess.stdout.on('data', suggestedPath => {
            output += suggestedPath.toString();
        });

        aStarProcess.stderr.on('data', () => {
            reject('Invalid program usage');
        });

        aStarProcess.on('close', (code) => {
            try {
                if (code !== 0) {
                    reject('Program returned error code');
                    return;
                }
                const lines = output.trim().split('\n');
                const pathLine = lines.findIndex(line => line.startsWith('Optimized Path:')) + 1;

                const routeLines = lines.filter(line => line.includes('via'));

                const path = lines[pathLine]
                    .split(' -> ')
                    .map(city => city.trim());

                const parsedRoutes = routeLines.map((line) => parseRouteString(line));

                resolve({
                    path,
                    route: parsedRoutes 
                });
            } catch (err) {
                reject(`Failed to load a* output: ${err}`);
            }
        })

        aStarProcess.on('error', (error) => {
            reject(`Failed to start process: ${error.message}`);
        })
    })
}

/**
 * Retrieves a efficient knapsack solution
 * @param capacity: Capacity of the sack
 * @param items: Array of object
 * [
 * {
 *      id: number,
 *      capacity: number,
 *      volume: number,
 * }
 * ]
 *
 * @todo add filtering on items that are approaching deadline
 */
export const getKnapsackSolution = (capacity, items, volume) => {
    return new Promise((resolve, reject) => {
        try {
            const args = [capacity.toString(), volume.toString(), items.length.toString()];
            for (const item of items) {
                args.push(item.id.toString(), item.weight.toString(), item.volume.toString(), calculatePriority(item).toString());
            }
            const knapsackProcess = spawn(knapsack_file_path, args);

            knapsackProcess.stdout.on('data', (data) => {
                resolve(data.toString().trim());
            })

            knapsackProcess.stderr.on('data', (data) => {
                reject('Invalid program usage');
                console.error(data);
            })

            knapsackProcess.on('close', (code) => {
                if (code !== 0) {
                    reject('Program returned error code');
                    return;
                }
            })

        } catch (err) {
            reject(`Failed to load knapsack solution: ${err}`)
        }
    })
}

/**
 * Generates the priority of the delivery
 * @param product
 * {
 *      product_name: string,
 *      assigned_delivery: string,
 *      sender: string,
 *      recipient: string,
 *      destination: string,
 *      source: string,
 *      date_shipped: date,
 *      deadline: timestamp,
 *      weight: number,
 *      volume: number
 *      status: delivery_status
 * }
 *
 * @returns score (0 - 80)
 */
const calculatePriority = (product) => {
    const now = Date.now();
    const deadline = product.deadline;
    const shipped = product.shipped;

    const daysToDeadline = (deadline - now) / (1000 * 60 * 60 * 24);
    const daysSinceShipped = (now - shipped) / (1000 * 60 * 60 * 24);

    let deadlineScore = 0;
    switch (true) {
        case daysToDeadline < 1:
            deadlineScore = 50;
            break;
        case daysToDeadline < 2:
            deadlineScore = 40;
            break;
        case daysToDeadline < 3:
            deadlineScore = 30;
            break;
        default:
            deadlineScore = 10;
    }

    let delayScore = 0;
    switch (true) {
        case daysSinceShipped > 3:
            delayScore = 30;
            break;
        case daysSinceShipped > 2:
            delayScore = 20;
            break;
        case daysSinceShipped > 1:
            delayScore = 10;
            break;
        default:
            delayScore = 0;
    }

    const totalScore = deadlineScore + delayScore;
    return totalScore;
}

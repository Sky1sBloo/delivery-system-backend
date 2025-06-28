import { spawn } from 'child_process';

const a_star_file_path = './dist/a_star';
const knapsack_file_path = './dist/knapsack';

// Must be relative to the program
const graph_file = './src/paths/graph.txt'
const heuristic_file = './src/paths/heuristic.txt'
const city_mapping_file = './src/paths/city_mapping.txt'

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

                // Removes the optimized path header
                console.log(lines, "\nLine: \n", lines[pathLine]);
                const path = lines[pathLine]
                    .split(' -> ')
                    .map(city => city.trim());

                resolve({
                    path,
                    route: routeLines
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
 *      weight: number,
 *      value: number
 * }
 * ]
 *
 * @todo add filtering on items that are approaching deadline
 */
export const getKnapsackSolution = (capacity, items) => {
    return new Promise((resolve, reject) => {
        try {
            const args = [capacity.toString(), items.length.toString()];
            for (const item of items) {
                args.push(item.id.toString(), item.weight.toString(), item.value.toString());
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

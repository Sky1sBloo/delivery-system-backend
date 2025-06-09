import { spawn } from 'child_process';

const a_star_file_path = './dist/a_star';
const knapsack_file_path = './dist/knapsack';

// Must be relative to the program
const graph_file = './src/paths/graph.txt'
const heuristic_file = './src/paths/heuristic.txt'

/*
 * Suggests a delivery route
 * @param source: integer Node representing the source 
 * @param destination: integer Note representing the destination
 */
export const suggestDeliveryRoute = (source, destination) => {
    return new Promise((resolve, reject) => {
        const aStarProcess = spawn(a_star_file_path, [graph_file, heuristic_file, source, destination]);
        let output = -1;
        aStarProcess.stdout.on('data', suggestedPath => {
            output = suggestedPath.toString().trim();
        });

        aStarProcess.stderr.on('data', () => {
            reject('Invalid program usage');
        });

        aStarProcess.on('close', (code) => {
            if (code === 0) {
                resolve(output);
            } else {
                reject('Program returned error code');
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
 * id: number,
 * weight: number
 * }
 * ]
 *
 * @todo add filtering on items that are approaching deadline
 */
export const getKnapsackSolution = (capacity, items) => {
}

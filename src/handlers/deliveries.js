import { spawn } from 'child_process';
const program_file_path = '../../dist/a_star';
const graph_file = '../paths/graph.txt'
const heuristic_file = '../paths/heuristic_file.txt'

/*
 * Suggests a delivery route
 * @param source: integer Node representing the source 
 * @param destination: integer Note representing the destination
 */
export const suggestDeliveryRoute = (source, destination) => {
    return new Promise((resolve, reject) => {
        const aStarProcess = spawn(program_file_path, graph_file, heuristic_file, source, destination);
        const output = -1;
        aStarProcess.stdout.on('data', suggestedPath => {
            resolve();
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
    })
}

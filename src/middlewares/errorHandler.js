import { getStatusCode } from "../supabaseStatusCodes.js";

export class DataSizeError extends Error {
    constructor() {
        super('Data is empty or data length is not 1');
    }
}

export class DBNonexistentData extends Error {
    constructor() {
        super('Data return empty')
    }
}

export const errorHandler = (error, _req, res, _next) => {
    if (error && typeof error.code === 'string' && typeof error.message === 'string') {
        const postgresError = error;
        res.status(getStatusCode(postgresError.code)).json({ message: postgresError.message });
        return;
    }
    if (error instanceof DBNonexistentData) {
        res.status(404).json({ message: 'Cannot retrieve nonexistent data' });
        return;
    }
    console.error(error);
    res.status(500).json({ message: 'Internal server error' });
}

export const errorHandlerOld = (error, _, res, _next) => {
    console.error(error);
    return res.status(500).json({ message: 'Internal Server Error' });
}

export const errorHandler = (error, _, res, _next) => {
    console.error(error);
    return res.status(500).json({ message: 'Internal Server Error' });
}

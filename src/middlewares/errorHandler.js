export const errorHandler = (error, _, res) => {
    console.error(error);
    return res.status(500).json({ message: 'Internal Server Error' });
}

import express from 'express';
import { errorHandler } from './middlewares/errorHandler';

const app = express();

const port = process.env.PORT || 3000;
app.listen(port, () => {
    console.log(`Server running on port ${port}`);
});

app.use(errorHandler);

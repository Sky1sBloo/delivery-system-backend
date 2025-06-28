import express from 'express';
import { errorHandler } from './middlewares/errorHandler.js';
import deliveryRouter from './routes/deliveryRoutes.js';
import userRouter from './routes/userRoutes.js';
import session from 'express-session';

const app = express();

const port = process.env.PORT || 3000;
app.listen(port, () => {
    console.log(`Server running on port ${port}`);
});

app.use(express.json());
app.use(session({
    secret: process.env.SESSION_SECRET || 'forgot ur .env file',
    resave: false,
    saveUninitialized: false
}));

app.use('/api/delivery', deliveryRouter);
app.use('/api/user', userRouter);
app.use(errorHandler);

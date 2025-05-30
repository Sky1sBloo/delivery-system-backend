import { Router } from "express";
import { loginUser, logoutUser, registerUser } from "../controllers/userController";

const router = Router();

router.use('/register', registerUser);
router.use('/login', loginUser);
router.use('/logout', logoutUser);

export default router;

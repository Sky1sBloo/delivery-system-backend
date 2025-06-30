import { Router } from "express";
import { getDeliveryUsers, getUserInfo, loginUser, logoutUser, registerUser } from "../controllers/userController.js";
import { isAuthenticated, isManagement } from "../middlewares/authentication.js";

const router = Router();

router.use('/register', registerUser);
router.use('/login', loginUser);
router.use('/logout', logoutUser);
router.get('/', isAuthenticated, getUserInfo);
router.get('/delivery', isManagement, getDeliveryUsers);

export default router;

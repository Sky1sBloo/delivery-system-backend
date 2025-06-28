import { Router } from 'express';
import { addDelivery, deleteDelivery, editDeliveryInfo, retrieveDeliveries, getDeliveryRoute, suggestDeliveryItems, retrieveOwnedDeliveries } from '../controllers/deliveryController.js';
import { isDelivery, isManagement } from '../middlewares/authentication.js';

const router = Router();

router.get('/all', isManagement, retrieveDeliveries);
router.get('/', isDelivery, retrieveOwnedDeliveries);
router.post('/', isManagement, addDelivery);
router.put('/:id', isManagement, editDeliveryInfo);
router.delete('/:id', isManagement, deleteDelivery);
router.get('/route', isDelivery, getDeliveryRoute);
router.get('/items', isManagement, suggestDeliveryItems);

export default router;

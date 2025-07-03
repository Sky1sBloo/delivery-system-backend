import { Router } from 'express';
import { addDelivery, deleteDelivery, editDeliveryInfo, retrieveDeliveries, getDeliveryRoute, suggestDeliveryItems, retrieveOwnedDeliveries, retrieveDeliveriesById } from '../controllers/deliveryController.js';
import { isDelivery, isManagement } from '../middlewares/authentication.js';

const router = Router();

router.get('/all', isManagement, retrieveDeliveries);
router.get('/', isDelivery, retrieveOwnedDeliveries);
router.get('/route', isDelivery, getDeliveryRoute);
router.get('/items', isManagement, suggestDeliveryItems);
router.get('/:id', isManagement, retrieveDeliveriesById);
router.post('/', isManagement, addDelivery);
router.put('/:id', isManagement, editDeliveryInfo);
router.delete('/:id', isManagement, deleteDelivery);

export default router;

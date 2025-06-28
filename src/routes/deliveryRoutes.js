import { Router } from 'express';
import { addDelivery, deleteDelivery, editDeliveryInfo, retrieveDeliveries, getDeliveryRoute, suggestDeliveryItems, retrieveOwnedDeliveries } from '../controllers/deliveryController.js';
import { isDelivery, isManagement } from '../middlewares/authentication.js';

const router = Router();

router.get('/all', retrieveDeliveries);
router.get('/', isDelivery, retrieveOwnedDeliveries);
router.post('/', isManagement, addDelivery);
router.put('/:id', isManagement, editDeliveryInfo);
router.delete('/:id', deleteDelivery);
router.get('/route', getDeliveryRoute);
router.get('/items', suggestDeliveryItems);

export default router;

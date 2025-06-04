import { Router } from 'express';
import { addDelivery, deleteDelivery, editDeliveryInfo, retrieveDeliveries, getDeliveryRoute } from '../controllers/deliveryController.js';

const router = Router();

router.get('/', retrieveDeliveries);
router.post('/', addDelivery);
router.put('/:id', editDeliveryInfo);
router.delete('/:id', deleteDelivery);
router.get('/route', getDeliveryRoute);

export default router;

import { Router } from 'express';
import { addDelivery, deleteDelivery, editDeliveryInfo, retrieveDeliveries, suggestDeliveryRoute } from '../controllers/deliveryController';

const router = Router();

router.get('/', retrieveDeliveries);
router.post('/', addDelivery);
router.put('/:id', editDeliveryInfo);
router.delete('/:id', deleteDelivery);
router.get('/route', suggestDeliveryRoute);

export default router;

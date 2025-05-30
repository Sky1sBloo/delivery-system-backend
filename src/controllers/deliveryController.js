import supabase from '../database.js';
/**
 * Addes a new delivery
 * @param req.body
 * {
 *      product_name: string,
 *      sender: string,
 *      recipient: string,
 *      destination: string,
 *      source: string,
 *      date_shipped: date,
 *      deadline: timestamp,
 *      status: delivery_status
 * }
 */
export const addDelivery = async (req, res, next) => {
    const requiredParams = [req.body.product_name, req.body.recipient, req.body.destination, req.body.source, req.body.deadline];
    try {
        for (const value of requiredParams) {
            if (value == undefined) {
                return res.status(400).json({ message: 'Missing body parameters' });
            }
        }
        const { error } = supabase.from('delivery').insert(req.body);
        if (error) {
            throw new Error(error);
        }

        return res.status(201).json({message: 'Created new delivery'});
    } catch (error) {
        next(error);
    }
}

/**
 * Addes a new delivery
 * @param req.body
 * {
 *      product_name: string,
 *      sender: string,
 *      recipient: string,
 *      destination: string,
 *      source: string,
 *      date_shipped: date,
 *      deadline: timestamp,
 *      status: delivery_status
 * }
 */
export const editDeliveryInfo = (req, res, next) => {
}

export const deleteDelivery = (req, res, next) => {
}

export const suggestDeliveryRoute = (req, res, next) => {
}

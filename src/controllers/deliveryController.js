import supabase from '../database.js';
import { getKnapsackSolution, suggestDeliveryRoute } from '../handlers/deliveries.js';


/**
 * Retrieves all possible deliveries
 */
export const retrieveDeliveries = async (_, res, next) => {
    try {
        const { data, error } = await supabase.from('delivery').select();
        if (error) {
            throw new Error(error.message);
        }
        return res.status(200).send(data);
    } catch (error) {
        next(error);
    }
}

/**
 * Retrieve specific delivery by id
 */
export const retrieveDeliveriesById = async (req, res, next) => {
    if (!req.params || req.params.id === undefined) {
        return res.status(400).json({ message: 'Missing params id' });
    }
    try {
        const { data, error } = await supabase.from('delivery').select().eq('id', req.params.id);
        if (error) {
            throw new Error(error.message);
        }
        return res.status(200).send(data[0]);
    } catch (error) {
        next(error);
    }
}

/**
 * @param req.session
 * {
 *  username
 *  accountType
 * }
 */
export const retrieveOwnedDeliveries = async (req, res, next) => {
    try {
        const username = req.session.username;
        if (username === undefined) {
            throw new Error('Username is undefined');
        }
        const { data, error } = await supabase.from('delivery').select().eq('assigned_delivery', username);
        if (error) {
            throw new Error(error.message);
        }
        return res.status(200).send(data);

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
 *      destination: number,
 *      source: number,
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
        const { error } = await supabase.from('delivery').insert(req.body).select();
        if (error) {
            throw new Error(error.message);
        }

        return res.status(201).json({ message: 'Created new delivery' });
    } catch (error) {
        next(error);
    }
}

/**
 * Edits an existing delivery
 * @param req.body
 * {
 *      product_name: string,
 *      assigned_delivery: string,
 *      sender: string,
 *      recipient: string,
 *      destination: string,
 *      source: string,
 *      date_shipped: date,
 *      deadline: timestamp,
 *      status: delivery_status
 * }
 *
 * @param req.params
 * id: number
 */
export const editDeliveryInfo = async (req, res, next) => {
    const { error } = await supabase.from('delivery').update(req.body).eq('id', req.params.id);
    if (error) {
        next(error);
        return;
    }

    return res.status(200).json({ message: 'Updated delivery' });
}
/**
 * Deletes an existing delivery
 * @param req.param
 * {
 *      id: number,
 * }
 */
export const deleteDelivery = async (req, res, next) => {
    if (!req.params || req.params.id == undefined) {
        return res.status(400).json({ message: 'Missing body param: id' });
    }
    const { error } = await supabase.from('delivery').delete().eq('id', req.params.id);
    if (error) {
        next(error);
        return;
    }

    return res.status(200).json({ message: 'Deleted delivery' });
}

/**
 * Suggests a delivery route and returns its recommended path
 *
 * @param req.body
 * {
 *      source: number,
 *      destination: number
 * }
 */
export const getDeliveryRoute = async (req, res, next) => {
    if (!req.body || req.body.source === undefined || req.body.destination === undefined) {
        return res.status(400).json({ message: 'Missing source/destination' });
    }

    try {
        const suggestedPath = await suggestDeliveryRoute(req.body.source, req.body.destination);
        return res.status(200).json(suggestedPath);
    } catch (error) {
        next(error);
    }
}

/**
 * Suggests the items fit for the truck
 * @param req.body
 * {
 *      source: number,
 *      destination: number,
 *      capacity: number
 * }
 */
export const suggestDeliveryItems = async (req, res, next) => {
    const { source, destination, capacity } = req.body;

    if (source === undefined || destination === undefined || capacity === undefined) {
        return res.status(400).json({ message: 'Missing required body params' });
    }

    try {
        const { data, error } = await supabase
            .from('delivery')
            .select()
            .eq('destination', req.body.destination)
            .eq('source', req.body.source)
            .eq('status', 'pending');

        if (error) {
            throw new Error(error.message);
        }

        const items = await getKnapsackSolution(capacity, data);

        return res.status(200).json(items);

    } catch (error) {
        next(error);
    }
}

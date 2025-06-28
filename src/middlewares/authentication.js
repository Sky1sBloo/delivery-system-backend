import { AccountType } from "../models/users.js";
/**
 * Checks if the account is authenticated
 */
export const isAuthenticated = (req, res, next) => {
    if (!req.session || !req.session.isLoggedIn) {
        return res.status(401).json({ message: 'Not authenticated' });
    }
    next();
}

/**
 * Checks if account is a management role
 * Note: Already performs authentication checking
 */
export const isManagement = (req, res, next) => {
    if (!req.session || !req.session.isLoggedIn) {
        return res.status(401).json({ message: 'Not authenticated' });
    }

    if (req.session.accountType === AccountType.MANAGEMENT) {
        next();
        return;
    }
    return res.status(401).json({ message: 'Account does not have privileges to access this content' });
}
/**
 * Checks if account is a delivery role
 * Note: Already performs authentication checking
 */
export const isDelivery = (req, res, next) => {
    if (!req.session || !req.session.isLoggedIn) {
        return res.status(401).json({ message: 'Not authenticated' });
    }
    console.log(req.session.accountType);

    if (req.session.accountType === AccountType.DELIVERY) {
        next();
        return;
    }
    return res.status(401).json({ message: 'Account does not have privileges to access this content' });
}

import { AccountType } from "../models/users";
/**
 * Checks if the account is authenticated
 */
export const isAuthenticated = (_, res, next) => {
    if (express.session.isLoggedIn === true) {
        next();
    } else {
        return res.status(401).json({ message: 'Not authenticated' });
    }
}

/**
 * Checks if account is a management role
 * Note: Already performs authentication checking
 */
export const isManagement = (_, res, next) => {
    if (express.session.isLoggedIn === false) {
        return res.status(401).json({ message: 'Not authenticated' });
    }

    if (express.session.accountType === AccountType.MANAGEMENT) {
        next();
    }
    return res.status(401).json({ message: 'Account does not have privileges to access this content' });
}
/**
 * Checks if account is a delivery role
 * Note: Already performs authentication checking
 */
export const isDelivery = (_, res, next) => {
    if (express.session.isLoggedIn === false) {
        return res.status(401).json({ message: 'Not authenticated' });
    }

    if (express.session.accountType === AccountType.DELIVERY) {
        next();
    }
    return res.status(401).json({ message: 'Account does not have privileges to access this content' });
}

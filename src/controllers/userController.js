/*
 * Contains functions for the user controller
 * NOTE: See models/users.js for express session information
 */

import bcrypt from 'bcrypt';
import supabase from '../database.js';

/**
 * Registers the user
 *
 * @param req
 * {
 *   username: string,
 *   password: string,
 *   accountType: 'management' || 'delivery'
 * }
 */
export const registerUser = async (req, res, next) => {
    try {
        const username = req.body.username;
        const password = req.body.password;
        const accountType = req.body.accountType;

        if (!username || !password) {
            return res.status(400).json({ message: 'Missing parameters' });
        }

        if (accountType !== 'management' && accountType !== 'delivery') {
            return res.status(400).json({ message: 'Invalid user type' });
        }

        const usernameSQL = await supabase.from('users').select('username').eq('username', username);
        if (usernameSQL.data == null || usernameSQL.error != null) {
            throw new Error(usernameSQL.error.message);
        }

        if (usernameSQL.data.length !== 0) {
            return res.status(409).json({ message: 'Username already exists' });
        }

        const hashedPassword = await bcrypt.hash(password, 10);
        const { error } = await supabase.from('users').insert({
            username: username,
            password: hashedPassword,
            account_type: accountType
        });


        if (error != null) {
            console.log(error);
            throw new Error(error.message);
        }
        req.session.isLoggedIn = true;
        req.session.username = username;
        req.session.accountType = accountType;
        return res.status(201).json({ message: `Successfully registered user ${username}` });
    } catch (error) {
        next(error);
    }
}

/**
 * Logins the user
 *
 * @param req
 * {
 *   username: string,
 *   password: string,
 * }
 */
export const loginUser = async (req, res, next) => {
    try {
        const username = req.body.username;
        const password = req.body.password;

        if (!username || !password) {
            return res.status(400).json({ message: 'Missing parameters' });
        }

        const { data, error } = await supabase.from('users').select('password, account_type').eq('username', username);
        if (error) {
            throw new Error(error.message);
        }
        if (data.length != 1) {
            throw new Error('Username more than 1 exists');
        }

        const user = data[0];
        const passwordMatch = await bcrypt.compare(password, user.password);
        if (!passwordMatch) {
            return res.status(401).json({ message: 'Wrong username or password' });
        }
        req.session.username = username;
        req.session.accountType = user.account_type;
        req.session.isLoggedIn = true;

        return res.status(200).json({ message: 'Logged in' });
    } catch (error) {
        next(error);
    }
}

/**
 * Logs out the user
 */
export const logoutUser = (req, res) => {
    req.session.isLoggedIn = false;
    req.session.username = null;
    req.session.accountType = null;

    return res.status(200).send();
}

/**
 * Gets the user info of current logged in user
 */
export const getUserInfo = (req, res) => {
    const userInfo = {
        username: req.session.username,
        accountType: req.session.accountType
    };
    return res.status(200).json(userInfo);
}

export const getDeliveryUsers = async (_, res, next) => {
    const { data, error } = await supabase.from('users').select('username, account_type').eq('account_type', 'delivery');

    if (error) {
        next(error.message);
    }

    const accounts = data.map(({ username, account_type }) => ({
        username,
        accountType: account_type
    }));


    res.status(200).json(accounts);
}

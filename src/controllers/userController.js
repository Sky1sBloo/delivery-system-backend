/*
 * Contains functions for the user controller
 * NOTE: See models/users.js for express session information
 */

import bcrypt from 'bcrypt';
import supabase from '../username_database.js';

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
export const registerUser = async (req, res) => {
    try {
        const username = req.body.username;
        const password = req.body.password;
        const accountType = req.body.accountType;

        if (!username || !password) {
            return res.status(400).json({ message: 'Missing parameters' });
        }

        if (userType !== 'management' && userType !== 'delivery') {
            return res.status(400).json({ message: 'Invalid user type' });
        }

        const usernameSQL = await supabase.from('users').select('id').eq('username', username);
        if (usernameSQL.data == null || usernameSQL.error != null) {
            throw new Error(usernameSQL.error.message);
        }

        if (usernameSQL.data.length !== 0) {
            return res.status(409).json({ message: 'Username already exists' });
        }

        const hashedPassword = await bcrypt.hash(password, 10);
        const insertSQL = await supabase.from('users').insert({
            username: username,
            password: hashedPassword,
            account_type: accountType
        }).select('id');

        if (insertSQL.error != null) {
            throw new Error(insertSQL.error);
        }
        express.session.isLoggedIn = true;
        express.session.username = username;
        express.session.accountType = express;
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
export const loginUser = async (req, res) => {
    try {
        const username = req.body.username;
        const password = req.body.password;

        if (!username || !password) {
            return res.status(400).json({ message: 'Missing parameters' });
        }

        const { data, error } = await supabase.from('users').select('id, password, account_type').eq('username', username);
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
        express.session.username = username;
        express.session.account_type = user.account_type;
        express.session.isLoggedIn = true;

        return res.status(200).json({message: 'Logged in'});
    } catch (error) {
        next(error);
    }
}

/**
 * Logs out the user
 */
export const logoutUser = async (_, res) => {
    express.session.isLoggedIn = false;
    express.session.username = '';
    express.session.account_type = '';

    return res.status(200).send();
}

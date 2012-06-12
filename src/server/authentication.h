/* *************************************************************************
 *  Copyright 2012 TU VIENNA SEPM GROUP 22                                 *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */


#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <QString>
#include <QByteArray>

#include "SecureDistributedChat.h"

namespace sdcs
{

class Server;
class Session;

struct SessionContainer {
    Session *session;
    sdc::SessionIPrx proxy;
};

/**
 * The one and only Authentication instance.
 * It is used by the server for handling authentication requests from all clients.
 */
class Authentication : public sdc::AuthenticationI
{
public:
    Authentication();

    /**
     * Register a new user; see SecureDistributedChat.ice.
     * The password must be in plain text.
     */
    void registerUser(const sdc::User &participant, const std::string &pwd, const Ice::Current &)
    throw(sdc::AuthenticationException);

    /**
     * Login a new user; see SecureDistributedChat.ice.
     * 'ident' must be an implementation of sdc::SessionI.
     */
    sdc::SessionIPrx login(const sdc::User &participant, const std::string &pwd,
                           const Ice::Identity &ident, const Ice::Current &)
    throw(sdc::AuthenticationException);

    /**
     * Echo a message back to the client for testing purposes; see SecureDistributedChat.ice.
     */
    std::string echo(const std::string &message, const Ice::Current &)
    throw(sdc::SDCException);

private:
    /**
     * Generate random salt for additional password entropy.
     */
    QByteArray generatePasswordSalt() const;

    /**
     * Hash a password given the salt provided.
     */
    QByteArray saltHashPassword(const std::string &password, const QByteArray &salt) const;
};

}

#endif

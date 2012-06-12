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


/**
 * @file sessionmanager.h
 *
 * Guidelines:
 *
 * Every function in the backend is to be called asynchronous,
 * that is it returns immediately and the result will be communicated
 * using a signal. Make sure you're subscribed to the signal if you
 * are interested in it.
 * Functions will execute in their own threads, so the caller does not
 * need to care about blocking. One has to pay attention not to confuse
 * several result signals, as they don't always carry information on which
 * function call they belonged to. If there is potential for confusion
 * then the caller is not allowed to call the same function again before
 * the result is received.
 *
 * Usually the signal will be named functionNameCompleted, as in
 *   function:  SessionManager.login()
 *   signal:    SessionManager.loginCompleted()
 *
 * Functions return void. Result signals carry first their data and then
 * two standardized parameters indicating success and an error message (if
 * there is one).
 *   example:
 *      void loginCompleted(QSharedPointer<Session> session,
 *                          bool success,
 *                          const QString &msg);
 *
 * If success is false, then msg will always carry an error message. If success
 * is true then the content of msg is undefined.
 */
#ifndef _SESSIONMANAGER_H
#define _SESSIONMANAGER_H

#include <QSharedPointer>
#include <Ice/Ice.h>

#include "session.h"
#include "user.h"
#include "loginuser.h"

namespace sdcc
{

/**
 * Top level object for the client.
 * Normally, there will only be one session manager.
 * Maybe it should be a singleton?
 */
class SessionManager : public QObject
{
    Q_OBJECT

public:

    /**
     * Calls echo on the server to check its availability.
     */
    static void testConnection(const QString &serverName);

    /**
     * Login a user.
     * serverCertPath is the local path to the server's public key.
     */
    static void login(const QString &serverName,
                      const QString &serverCertPath, QSharedPointer<const LoginUser> usr,
                      const QString &pwd);

    /**
     * Register a new user on the server.
     */
    static void registerUser(const QString &serverName,
                             const QString &serverCertPath, QSharedPointer<const User> usr,
                             const QString &pwd);

    /**
     * Returns the singleton instance of SessionManager.
     */
    static SessionManager *getInstance();

signals:

    /**
     * ConnectionTest has completed.
     */
    void testConnectionCompleted(bool success, const QString &msg);

    /**
     * Login has completed.
     * session handles all further communication with the server.
     */
    void loginCompleted(QSharedPointer<Session> session, bool success,
                        const QString &msg);

    void registerCompleted(bool success, const QString &msg);


private:
    /* Prevent unintended construction of instances by user. */
    SessionManager() { }
    SessionManager(const SessionManager &);

    static SessionManager instance;

    /**
     * Internal method for testConnection to hand over to QtConcurrent
     */
    void runTestConnection(const QString &serverName);
    void runLogin(const QString &serverName, const QString &serverCertPath,
                  QSharedPointer<const LoginUser> usr, const QString &pwd);
    void runRegisterUser(const QString &serverName,
                         const QString &serverCertPath,
                         QSharedPointer<const User> usr, const QString &pwd);
};

}

#endif /* _SESSIONMANAGER_H */

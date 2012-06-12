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


#ifndef SERVER_H
#define SERVER_H

#include <Ice/Ice.h>
#include <QMap>
#include <QMutex>
#include <QSharedPointer>

#include "authentication.h"
#include "chat.h"
#include "interserver.h"

namespace sdcs
{

class Server
{
public:
    /**
     * Creates a server instance using the specified communicator.
     */
    static void create(Ice::CommunicatorPtr communicator);

    /**
     * Returns a pointer to the server instance.
     */
    static Server &instance();

    /**
     * Adds the session to the session map.
     * If the user already owns an active session, throws an AuthenticationException.
     */
    void addSession(const QString &user, const SessionContainer &container)
    throw(sdc::AuthenticationException);

    /**
     * Removes the session from the session map.
     * If the map doesn't contain given session, a UserHandlingException is thrown.
     */
    void removeSession(const QString &user)
    throw(sdc::UserHandlingException);

    /**
     * Returns the server's hostname.
     */
    const QString &getHostname() const;

    /**
     * Creates a new local chat, adds it to the chats list, and returns a pointer to it.
     * The user creating the chat is added to its participant list.
     */
    QSharedPointer<Chat> createLocalChat(const sdc::User &user, sdc::ChatClientCallbackIPrx callback);

    /**
     * Adds the chat with id chatID to the session-local chat map of user.
     * The chat must already be in the server chat map.
     * Throws an exception if that client is not active.
     */
    void addChatTo(const QString &user, const QString &chatID)
    throw(sdc::UserHandlingException);

    /**
     * Adds the remote chat with id chatID to the session-local chat map of user.
     * The remote chat is treated as something previously unknown to us and owned
     * by the user.
     * Throws an exception if that client is not active.
     */
    void addRemoteChatTo(const QString &user, const QString &chatID)
    throw(sdc::UserHandlingException);

    /**
     * Retrieve the user client callback.
     * Throws an exception if that client is not active.
     */
    sdc::ChatClientCallbackIPrx getClientCallback(const QString &user)
    throw(sdc::UserHandlingException);

    /**
     * Get the chat with id chatID.
     * Throws an InterServerException if the chat does not exist.
     */
    QSharedPointer<Chat> getChat(const QString &chatID)
    throw(sdc::InterServerException);

    /**
     * Removes the chat with chatID from the list of active chats.
     */
    void removeChat(const QString &chatID);

    /**
     * Returns an inter-server proxy for the requested hostname.
     */
    sdc::InterServerIPrx getInterServerProxy(const QString &hostname);

private:
    Server();
    virtual ~Server();

    /**
     * The singleton instance.
     */
    static Server server;

    /**
     * True if the instance has been initialized.
     */
    bool initialized;

    Ice::CommunicatorPtr communicator;
    Authentication *auth;
    InterServer *interserver;

    QString hostname;

    /**
     * Stores all active sessions by owning username.
     */
    QMap<QString, SessionContainer> sessions;
    QMutex sessionsMutex;

    /**
     * Stores all active chats by chat ID.
     */
    QMap<QString, QSharedPointer<Chat> > chats;
    QMutex chatsMutex;
};

}

#endif

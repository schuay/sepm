#ifndef SERVER_H
#define SERVER_H

#include <Ice/Ice.h>
#include <QMap>
#include <QMutex>
#include <QSharedPointer>

#include "authentication.h"
#include "chat.h"

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
     * Retrieve the user client callback. Throws an exception if that client is not active.
     */
    sdc::ChatClientCallbackIPrx addChatTo(const QString &user, const QString &chatID)
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

    /* TODO: InterServer. */

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

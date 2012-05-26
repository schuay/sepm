#include "server.h"

#include <assert.h>
#include <QMutexLocker>

#include "session.h"

namespace sdcs
{

/**
 * Define the server singleton instance.
 */
Server Server::server;

void Server::create(Ice::CommunicatorPtr communicator, const QString &hostname)
{
    assert(communicator);
    assert(!server.initialized);

    server.hostname = hostname;
    server.communicator = communicator;

    /* Initialize the ice adapter.
     *
     * Note that while the name of the object adapter is not important,
     * the name of the Ice Identity matters - it is the name of the implemented
     * interface bar the trailing 'I'.
     */

    Ice::ObjectAdapterPtr adapter =
        server.communicator->createObjectAdapterWithEndpoints(
            "SDCServer", QString("ssl -p %1").arg(sdc::port).toStdString());

    server.auth = new Authentication();
    Ice::ObjectPtr authObj = server.auth;

    adapter->add(authObj, communicator->stringToIdentity("Authentication"));
    adapter->activate();
}

Server &Server::instance()
{
    return server;
}

const QString &Server::getHostname() const
{
    return hostname;
}

Server::Server() : initialized(false)
{
}

Server::~Server()
{
    communicator->destroy();
}

void Server::addSession(const QString &user, const SessionContainer &container)
throw(sdc::AuthenticationException)
{
    QMutexLocker locker(&sessionsMutex);
    if (sessions.contains(user)) {
        throw sdc::AuthenticationException("Already logged in.");
    }

    sessions[user] = container;
}

void Server::removeSession(const QString &user)
throw(sdc::UserHandlingException)
{
    QMutexLocker locker(&sessionsMutex);
    if (!sessions.contains(user)) {
        throw sdc::UserHandlingException("Not logged in.");
    }

    sessions.remove(user);
}

QSharedPointer<Chat> Server::createLocalChat(const sdc::User &user, sdc::ChatClientCallbackIPrx callback)
{
    QString nameTemplate = QString("chat%1@") + hostname;
    QString name;

    QMutexLocker locker(&chatsMutex);

    /* Determine first available chat name. */
    for (int i = 0; ; i++) {
        name = nameTemplate.arg(i);
        if (!chats.contains(name))
            break;
    }
    QSharedPointer<Chat> p(new LocalChat(name, user, callback));

    chats[name] = p;

    return p;
}

sdc::ChatClientCallbackIPrx Server::addChatTo(const QString &user, const QString &chatID)
throw(sdc::UserHandlingException)
{
    QMutexLocker sessionsLocker(&sessionsMutex);
    if (!sessions.contains(user)) {
        throw sdc::UserHandlingException("User not logged in.");
    }

    QMutexLocker chatsLocker(&chatsMutex);
    if (!chats.contains(chatID)) {
        throw sdc::UserHandlingException("Chat does not exist.");
    }

    return sessions[user].session->addChat(chats[chatID]);
}

}

#include "server.h"

#include <assert.h>
#include <QMutexLocker>

#include "session.h"
#include "settings.h"

namespace sdcs
{

/**
 * Define the server singleton instance.
 */
Server Server::server;

void Server::create(Ice::CommunicatorPtr communicator)
{
    assert(communicator);
    assert(!server.initialized);

    server.hostname = sdc::Settings::getValue(sdc::Settings::SHostname).toString();
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

    server.interserver = new InterServer();
    Ice::ObjectPtr interServerObj = server.interserver;

    adapter->add(interServerObj, communicator->stringToIdentity("InterServer"));

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
    if (communicator)
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

void Server::addChatTo(const QString &user, const QString &chatID)
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

    sessions[user].session->addChat(chats[chatID]);
}

sdc::ChatClientCallbackIPrx Server::getClientCallback(const QString &user)
throw(sdc::UserHandlingException)
{
    QMutexLocker sessionsLocker(&sessionsMutex);
    if (!sessions.contains(user)) {
        throw sdc::UserHandlingException("User not logged in.");
    }

    return sessions[user].session->getCallback();
}

void Server::removeChat(const QString &chatID)
{
    QMutexLocker chatsLocker(&chatsMutex);
    if (!chats.contains(chatID)) {
        throw sdc::UserHandlingException("Chat does not exist.");
    }

    chats.remove(chatID);
}

QSharedPointer<Chat> Server::getChat(const QString &chatID)
throw(sdc::InterServerException)
{
    QMutexLocker chatsLocker(&chatsMutex);
    if (!chats.contains(chatID)) {
        throw sdc::InterServerException("Chat does not exist.");
    }

    return chats[chatID];
}

sdc::InterServerIPrx Server::getInterServerProxy(const QString &hostname)
{
    /* Spawning a new interserver proxy per request is primitive. On demand, we can replace
     * this with a caching solution. */
    QString conn = QString("InterServer:ssl -h %1 -p %2").arg(hostname).arg(sdc::port);
    return sdc::InterServerIPrx::checkedCast(communicator->stringToProxy(conn.toStdString()));
}

}

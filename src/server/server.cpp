#include "server.h"

#include <assert.h>
#include <QMutexLocker>

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

    server.auth = new Authentication(&server);
    Ice::ObjectPtr authObj = server.auth;

    adapter->add(authObj, communicator->stringToIdentity("Authentication"));
    adapter->activate();
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

void Server::addSession(const QString &user, sdc::SessionIPrx session)
throw(sdc::AuthenticationException)
{
    QMutexLocker locker(&sessionsMutex);
    if (sessions.contains(user)) {
        throw sdc::AuthenticationException("Already logged in.");
    }

    sessions[user] = session;
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

QSharedPointer<Chat> Server::createLocalChat()
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

    QSharedPointer<Chat> p(new LocalChat(name));
    chats[name] = p;

    return p;
}

}

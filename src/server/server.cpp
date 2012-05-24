#include "server.h"

#include <assert.h>
#include <QMutexLocker>

namespace sdcs
{

Server::Server(Ice::CommunicatorPtr communicator, const QString &hostname)
    : hostname(hostname)
{
    assert(communicator);

    this->communicator = communicator;

    /* Initialize the ice adapter.
     *
     * Note that while the name of the object adapter is not important,
     * the name of the Ice Identity matters - it is the name of the implemented
     * interface bar the trailing 'I'.
     */

    Ice::ObjectAdapterPtr adapter =
        communicator->createObjectAdapterWithEndpoints(
            "SDCServer", QString("ssl -p %1").arg(sdc::port).toStdString());

    auth = new Authentication(this);
    Ice::ObjectPtr authObj = auth;

    adapter->add(authObj, communicator->stringToIdentity("Authentication"));
    adapter->activate();
}

const QString &Server::getHostname() const
{
    return hostname;
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

}

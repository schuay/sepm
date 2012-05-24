#ifndef SERVER_H
#define SERVER_H

#include <Ice/Ice.h>
#include <QMap>
#include <QMutex>

#include "authentication.h"

namespace sdcs
{

class Server
{
public:
    Server(Ice::CommunicatorPtr communicator, const QString &hostname);
    virtual ~Server();

    /**
     * Adds the session to the session map.
     * If the user already owns an active session, throws an AuthenticationException.
     */
    void addSession(const QString &user, sdc::SessionIPrx session)
    throw(sdc::AuthenticationException);

    /**
     * Returns the server's hostname.
     */
    const QString &getHostname() const;

private:
    Ice::CommunicatorPtr communicator;
    Authentication *auth;

    /* TODO: InterServer. */

    const QString hostname;

    /**
     * Stores all active sessions by owning username.
     */
    QMap<QString, sdc::SessionIPrx> sessions;
    QMutex sessionsMutex;
};

}

#endif

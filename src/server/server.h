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
    Server(Ice::CommunicatorPtr communicator);
    virtual ~Server();

    /**
     * Adds the session to the session map.
     * If the user already owns an active session, throws an AuthenticationException.
     */
    void addSession(const QString &user, sdc::SessionIPrx session)
    throw(sdc::AuthenticationException);

private:
    Ice::CommunicatorPtr communicator;
    Authentication *auth;

    /* TODO: InterServer. */

    /**
     * Stores all active sessions by owning username.
     */
    QMap<QString, sdc::SessionIPrx> sessions;
    QMutex sessionsMutex;
};

}

#endif

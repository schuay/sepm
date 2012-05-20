#ifndef SERVER_H
#define SERVER_H

#include <Ice/Ice.h>

#include "authentication.h"

namespace sdcs
{

class Server
{
public:
    Server(Ice::CommunicatorPtr communicator);
    virtual ~Server();

private:
    Ice::CommunicatorPtr communicator;
    Authentication *auth;

    /* TODO: InterServer. */
};

}

#endif

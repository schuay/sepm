#include "server.h"

#include <assert.h>

namespace sdcs
{

Server::Server(Ice::CommunicatorPtr communicator)
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

    auth = new Authentication();
    Ice::ObjectPtr authObj = auth;

    adapter->add(authObj, communicator->stringToIdentity("Authentication"));
    adapter->activate();
}

Server::~Server()
{
    communicator->destroy();
}

}

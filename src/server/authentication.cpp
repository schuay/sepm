#include "authentication.h"

namespace sdcs
{

std::string Authentication::echo(const std::string &message, const Ice::Current &)
throw(sdc::SDCException)
{
    return message;
}

sdc::SessionIPrx Authentication::login(const sdc::User &, const std::string &,
                                       const Ice::Identity &, const Ice::Current &)
throw(sdc::AuthenticationException)
{
    return NULL;
}

void Authentication::registerUser(const sdc::User &/*participant*/,
                                  const std::string &/*pwd*/, const Ice::Current &)
throw(sdc::AuthenticationException)
{

}

}

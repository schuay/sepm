#include "authentication.h"
#include <string>

namespace sdcs
{
Authentication::Authentication()
{
}


std::string Authentication::echo(const std::string &message, const Ice::Current&)
{
    return message;
}

SessionIPrx Authentication::login(const User &, const std::string &,
                                  const Ice::Identity &, const Ice::Current&)
{
    return NULL;
}

void Authentication::registerUser(const User &, const std::string &, const Ice::Current &)
{
    return;
}

}

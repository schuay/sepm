#include "SecureDistributedChat.h"

namespace sdcs
{

class Authentication : public sdc::AuthenticationI
{
public:
    void registerUser(const sdc::User &participant, const std::string &pwd, const Ice::Current &)
    throw(sdc::AuthenticationException);

    sdc::SessionIPrx login(const sdc::User &participant, const std::string &pwd,
                           const Ice::Identity &ident, const Ice::Current &)
    throw(sdc::AuthenticationException);

    std::string echo(const std::string &message, const Ice::Current &)
    throw(sdc::SDCException);
};

}

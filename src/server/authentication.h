#include "SecureDistributedChat.h"

namespace sdcs
{

class Authentication : public sdc::AuthenticationI
{
public:
    void registerUser(const sdc::User &participant, const std::string &pwd, const Ice::Current &);
    sdc::SessionIPrx login(const sdc::User &participant, const std::string &pwd,
                           const Ice::Identity &ident, const Ice::Current &);
    std::string echo(const std::string &message, const Ice::Current &);
};

}

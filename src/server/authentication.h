#include "SecureDistributedChat.h"

using namespace sdc;

class Authentication : public AuthenticationI
{
public:
    void registerUser(User participant, std::string pwd) throw AuthenticationException;
    SessionI *login(User participant, std::string pwd,
                    Ice::Identity ident) throw AuthenticationException;
    std::string echo(std::string message) throw SDCException;

private:
    /* Every resource is protected by mutexes. */
    QMap<QString, Chat> chats;
    QMap<QString, Session> sessions;
}

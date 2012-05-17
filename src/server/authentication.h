#include "SecureDistributedChat.h"
#include <QMap>

#include "chat.h"
// #include "session.h" -> uncomment as soon as plausible

using namespace sdc;

namespace sdcs
{

class Authentication : public AuthenticationI
{

public:
    Authentication();

    virtual void registerUser(const User &participant, const std::string &pwd, const Ice::Current &);
    virtual SessionIPrx login(const User &participant, const std::string &pwd,
                              const Ice::Identity &ident, const Ice::Current&);
    virtual std::string echo(const std::string &message, const Ice::Current&);

private:
    /* Every resource is protected by mutexes. */
    QMap<QString, Chat> chats;
    // uncomment as soon as Session is compilable
    // QMap<QString, Session> sessions;
};

}

#include <QString>

#include "SecureDistributedChat.h"

namespace sdcs
{

class UserDbProxy
{
public:
    static UserDbProxy createUser(sdc::User user, sdc::ByteSeq hash);
    static UserDbProxy getUser(QString username);

    sdc::User getUser() const;
    sdc::ByteSeq getHash() const;

    void deleteUser();

private:
    UserDbProxy();
    UserDbProxy(const UserDbProxy &);

    createConnection(); // in ctor (pooled)
    disconnect(); // in dtor
};

}

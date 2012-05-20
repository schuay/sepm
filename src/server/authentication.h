#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include "SecureDistributedChat.h"
#include <QString>
#include <QByteArray>

namespace sdcs
{

class Authentication : public sdc::AuthenticationI
{
public:
    Authentication();

    void registerUser(const sdc::User &participant, const std::string &pwd, const Ice::Current &)
    throw(sdc::AuthenticationException);

    sdc::SessionIPrx login(const sdc::User &participant, const std::string &pwd,
                           const Ice::Identity &ident, const Ice::Current &)
    throw(sdc::AuthenticationException);

    std::string echo(const std::string &message, const Ice::Current &)
    throw(sdc::SDCException);

private:
    QByteArray generatePasswordSalt() const;
    QByteArray saltHashPassword(const std::string &password, const QByteArray &salt) const;
};

}

#endif

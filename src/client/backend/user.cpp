#include "user.h"

namespace sdcc
{

const QString &User::getName() const
{
    const QString &ref = this->name;
    return ref;
}

User::User(const QString &name, const QString &publicKeyPath)
throw(sdc::SecurityException)
{
    this->name = name;
    sdc::Security s;
    this->publicKey = s.readPubKey(publicKeyPath);
}

User::User(const sdc::User &user)
{
    this->name = QString::fromStdString(user.ID);
    this->publicKey = user.publicKey;
}

User::User(const User &user)
{
    this->name = user.name;
    this->publicKey = user.publicKey;
}

QSharedPointer<sdc::User> User::getIceUser() const
{
    sdc::User *u = new sdc::User();
    u->ID = name.toStdString();
    u->publicKey = publicKey;
    return QSharedPointer<sdc::User>(u);
}

const sdc::ByteSeq User::encrypt(const sdc::ByteSeq data) const throw(sdc::SecurityException)
{
    sdc::Security s;
    return s.encryptRSA(publicKey, data);
}

bool User::verify(const sdc::ByteSeq data, const sdc::ByteSeq signature) const
throw(sdc::SecurityException)
{
    sdc::Security s;
    return s.verifyRSA(publicKey, data, signature);
}

}

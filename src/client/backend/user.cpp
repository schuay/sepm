#include "user.h"

namespace sdcc
{

const QString &User::getName() const
{
    const QString &ref = this->name;
    return ref;
}

User::User(const QString &name, const QString &certificatePath)
throw (sdc::SecurityException)
{
    this->name = name;
    sdc::Security s;
    this->certificate = s.readPubKey(certificatePath);
}

User::User(const sdc::User &user)
{
    this->name = QString::fromStdString(user.ID);
    this->certificate = user.publicKey;
}

QSharedPointer<sdc::User> User::getIceUser() const
{
    sdc::User *u = new sdc::User();
    u->ID = name.toStdString();
    u->publicKey = certificate;
    return QSharedPointer<sdc::User>(u);
}

}

#ifndef _USER_H
#define _USER_H

#include <QString>
#include <QSharedPointer>

#include "SecureDistributedChat.h"
#include "Security.h"

namespace sdcc
{

/**
 * A user on a chat server, consisting of a username in the form "user@server",
 * and a public key.
 */

class User
{
public:
    User() { } /* Added (temporarily?) for qRegisterMetatype. */
    User(const QString &name, const QString &publicKeyPath) throw(sdc::SecurityException);
    User(const sdc::User &user);
    User(const User &user);
    const sdc::ByteSeq encrypt(sdc::ByteSeq data) const throw(sdc::SecurityException);
    const QString &getName() const;

    QSharedPointer<sdc::User> getIceUser() const;

private:
    QString      name;
    sdc::ByteSeq publicKey;
};

}

#endif /* _USER_H */

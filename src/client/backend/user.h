#ifndef _USER_H
#define _USER_H

#include <QString>
#include <QSharedPointer>

#include "SecureDistributedChat.h"
#include <Security.h>

namespace sdcc
{

/**
 * A user on a chat server, consisting of a username in the form "user@server",
 * and a public key.
 */

class User
{
public:

    User(const QString &name, const QString &certificatePath)
    throw (sdc::SecurityException);
    User(const sdc::User &user);

    const QString &getName() const;

    QSharedPointer<sdc::User> getIceUser() const;

private:
    QString      name;
    sdc::ByteSeq certificate;
};

}

#endif /* _USER_H */

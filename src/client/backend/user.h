/* *************************************************************************
 *  Copyright 2012 TU VIENNA SEPM GROUP 22                                 *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */


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
    const sdc::ByteSeq encrypt(const sdc::ByteSeq data) const throw(sdc::SecurityException);
    bool verify(const sdc::ByteSeq data, const sdc::ByteSeq signature) const
    throw(sdc::SecurityException);
    const QString &getName() const;

    QSharedPointer<sdc::User> getIceUser() const;

private:
    QString      name;
    sdc::ByteSeq publicKey;
};

}

#endif /* _USER_H */

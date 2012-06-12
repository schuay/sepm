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


#ifndef _LOGINUSER_H
#define _LOGINUSER_H

#include "user.h"

namespace sdcc
{

/**
 * A local user. In addition to the normal User, LoginUser also needs a private
 * Key to decrypt and sign stuff. After the LoginUser is created the private key
 * MUST NOT leave the object ever again. If it has to be changed, a new
 * LoginUser has to be created.
 */
class LoginUser : public User
{
public:
    /**
     * Ctor, reads private key from file, takes optional passphrase for key.
     */
    LoginUser(const QString &name, const QString &publicKeyPath,
              const QString &privateKeyPath, const QString &passphrase = "")
    throw(sdc::SecurityException);
    LoginUser(const LoginUser &user);

    /**
     * We never need to read the private key, just use it.
     * Hence we try to contain it within LoginUser. Noone else should
     * have to know it.
     */
    const sdc::ByteSeq decrypt(sdc::ByteSeq data) const throw(sdc::SecurityException);
    const sdc::ByteSeq sign(sdc::ByteSeq data) const throw(sdc::SecurityException);

private:
    const sdc::ByteSeq privateKey;
};

}

#endif /* _LOGINUSER_H */

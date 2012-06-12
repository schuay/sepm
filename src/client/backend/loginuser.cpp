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


#include "loginuser.h"

namespace sdcc
{

LoginUser::LoginUser(const QString &name, const QString &publicKeyPath,
                     const QString &privateKeyPath, const QString &passphrase)
throw(sdc::SecurityException)
    : User(name, publicKeyPath),
      privateKey(sdc::Security().readPrivKey(privateKeyPath,
                 passphrase)) {}

LoginUser::LoginUser(const LoginUser &user) : User(user), privateKey(user.privateKey) {}

const sdc::ByteSeq LoginUser::decrypt(sdc::ByteSeq data) const throw(sdc::SecurityException)
{
    sdc::Security s;
    return s.decryptRSA(privateKey, data);
}

const sdc::ByteSeq LoginUser::sign(sdc::ByteSeq data) const throw(sdc::SecurityException)
{
    sdc::Security s;
    return s.signRSA(privateKey, data);
}

}

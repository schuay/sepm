#include "loginuser.h"

namespace sdcc
{

LoginUser::LoginUser(const QString &name, const QString &publicKeyPath,
                     const QString &privateKeyPath, const QString &passphrase)
throw (sdc::SecurityException)
    : User(name, publicKeyPath),
      privateKey(sdc::Security().readPrivKey(privateKeyPath,
                 passphrase)) {}

LoginUser::LoginUser(const LoginUser &user) : User(user), privateKey(user.privateKey) {}

const sdc::ByteSeq LoginUser::decrypt(sdc::ByteSeq data) const throw (sdc::SecurityException)
{
    sdc::Security s;
    return s.decryptRSA(privateKey, data);
}

const sdc::ByteSeq LoginUser::sign(sdc::ByteSeq data) const throw (sdc::SecurityException)
{
    sdc::Security s;
    return s.signRSA(privateKey, data);
}

}

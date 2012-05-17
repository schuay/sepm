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
    throw (sdc::SecurityException);
    LoginUser(const LoginUser &user);

    /**
     * We never need to read the private key, just use it.
     * Hence we try to contain it within LoginUser. Noone else should
     * have to know it.
     */
    const sdc::ByteSeq decrypt(sdc::ByteSeq data) const throw (sdc::SecurityException);
    const sdc::ByteSeq sign(sdc::ByteSeq data) const throw (sdc::SecurityException);

private:
    const sdc::ByteSeq privateKey;
};

}

#endif /* _LOGINUSER_H */

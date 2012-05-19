#include "authentication.h"

#include <QByteArray>
#include <QtCrypto>

#include "QsLog.h"
#include "userdbproxy.h"

namespace sdcs
{

Authentication::Authentication()
{
    assert(QCA::isSupported("sha1"));
}

QByteArray Authentication::generatePasswordSalt() const
{
    // guaranteed to be random and unique
    return QString("asdfjklo").toLocal8Bit();
}

QByteArray Authentication::saltHashPassword(const std::string &password, const QByteArray &salt) const
{
    QCA::Hash shaHash("sha1");

    shaHash.update(password.c_str(), password.size());
    shaHash.update(salt);

    return shaHash.final().toByteArray();
}

std::string Authentication::echo(const std::string &message, const Ice::Current &)
throw(sdc::SDCException)
{
    return message;
}

sdc::SessionIPrx Authentication::login(const sdc::User &user, const std::string &password,
                                       const Ice::Identity &, const Ice::Current &)
throw(sdc::AuthenticationException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(QString::fromStdString(user.ID));
        QByteArray salt = proxy->getSalt();
        QByteArray hash = saltHashPassword(password, salt);

        if (hash != proxy->getHash()) {
            QLOG_TRACE() << "Authentication failure";
            throw sdc::AuthenticationException("Authentication failure");
        }
    } catch (const sdc::UserHandlingException &e) {
        throw sdc::AuthenticationException(e.what);
    }

    // TODO: create session object!

    return NULL;
}

void Authentication::registerUser(const sdc::User &user,
                                  const std::string &password, const Ice::Current &)
throw(sdc::AuthenticationException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QByteArray salt = generatePasswordSalt();
    QByteArray hash = saltHashPassword(password, salt);

    try {
        UserDbProxy::createUser(user, hash, salt);
    } catch (const sdc::UserHandlingException &e) {
        throw sdc::AuthenticationException(e.what);
    }
}

}

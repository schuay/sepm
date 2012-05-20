#include "authentication.h"

#include <QByteArray>
#include <QtCrypto>

#include "QsLog.h"
#include "userdbproxy.h"
#include "session.h"
#include "server.h"

namespace sdcs
{

#define SALT_LEN (128)
#define SALT_ITERATIONS (1000)

Authentication::Authentication(Server *server)
    : server(server)
{
    assert(QCA::isSupported("sha1"));
}

QByteArray Authentication::generatePasswordSalt() const
{
    return QCA::Random::randomArray(SALT_LEN).toByteArray();
}

QByteArray Authentication::saltHashPassword(const std::string &password, const QByteArray &salt) const
{
    QCA::Hash shaHash("sha1");

    shaHash.update(password.c_str(), password.size());
    for (int i = 0; i < SALT_ITERATIONS; i++) {
        shaHash.update(salt);
    }

    return shaHash.final().toByteArray();
}

std::string Authentication::echo(const std::string &message, const Ice::Current &)
throw(sdc::SDCException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    return message;
}

sdc::SessionIPrx Authentication::login(const sdc::User &user, const std::string &password,
                                       const Ice::Identity &identity, const Ice::Current &current)
throw(sdc::AuthenticationException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        QString userid = QString::fromStdString(user.ID);
        QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(userid);
        QByteArray salt = proxy->getSalt();
        QByteArray hash = saltHashPassword(password, salt);

        if (hash != proxy->getHash()) {
            QLOG_TRACE() << "Authentication failure";
            throw sdc::AuthenticationException("Authentication failure");
        }

        sdc::ChatClientCallbackIPrx callback = sdc::ChatClientCallbackIPrx::uncheckedCast(
                current.con->createProxy(identity));

        Session *session = new Session(user, callback);
        sdc::SessionIPrx sessionProxy = sdc::SessionIPrx::uncheckedCast(
                                            current.adapter->addWithUUID(session));

        std::string msg = "hello world";
        if (callback->echo(msg) != msg)
            throw sdc::AuthenticationException("Invalid callback");

        server->addSession(userid, sessionProxy);

        return sessionProxy;
    } catch (const sdc::UserHandlingException &e) {
        throw sdc::AuthenticationException(e.what);
    } catch (const Ice::Exception &e) {
        throw sdc::AuthenticationException(e.what());
    }
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

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


#include "authentication.h"

#include <QByteArray>
#include <QtCrypto>

#include "QsLog.h"
#include "userdbproxy.h"
#include "session.h"
#include "server.h"
#include "sdcHelper.h"

namespace sdcs
{

#define SALT_LEN (128)
#define SALT_ITERATIONS (1000)

Authentication::Authentication()
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

        SessionContainer container;
        container.session = new Session(user, callback);
        container.proxy = sdc::SessionIPrx::uncheckedCast(
                              current.adapter->addWithUUID(container.session));

        std::string msg = "hello world";
        if (callback->echo(msg) != msg)
            throw sdc::AuthenticationException("Invalid callback");

        Server::instance().addSession(userid, container);

        return container.proxy;
    } catch (const sdc::AuthenticationException &e) {
	throw e;
    /* Distinguish between a nonexistent user and a DB error.
     * In the former case do the same as for an invalid password.
     * This is good security practice. */
    } catch (const NoSuchUserException &e) {
	throw sdc::AuthenticationException("Authentication failure");
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

    const QString ourHostname = Server::instance().getHostname();
    const QString usrHostname = QString::fromStdString(sdc::sdcHelper::getServerFromID(user.ID));
    if (ourHostname != usrHostname) {
        throw sdc::AuthenticationException(QString("Cannot register user %1 on another server (hostname is \"%2\").")
                                           .arg(QString::fromStdString(user.ID)).arg(ourHostname)
                                           .toStdString());
    }

    QByteArray salt = generatePasswordSalt();
    QByteArray hash = saltHashPassword(password, salt);

    try {
        UserDbProxy::createUser(user, hash, salt);
    } catch (const sdc::UserHandlingException &e) {
        throw sdc::AuthenticationException(e.what);
    }
}

}

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


#include "sessionmanager.h"

#include <QtConcurrentRun>

#include "SecureDistributedChat.h"
#include "QsLog.h"

namespace sdcc
{

/**
 * Private wrapper around Ice::CommunicatorPtr which handles common connection
 * setup and ensures that destroy() is called when the wrapper goes out of scope.
 */
class CommunicatorPtrWrapper
{
public:
    enum Option {
        NoOptions = 0x00,
        DontVerifyPeer = 1 << 1,
        DontDestroyComm = 1 << 2
    };
    Q_DECLARE_FLAGS(Options, Option)

    CommunicatorPtrWrapper(const QString &serverName,
                           const QString &serverCertPath,
                           Options options, bool &success, QString &msg) {
        msg = "";
        success = true;
        opts = options;

        QString conn = QString("Authentication:ssl -h %1 -p %2")
                       .arg(serverName).arg(sdc::port);

        /* Set up ice to initialize ssl plugin and set used cert file. */
        Ice::PropertiesPtr props = Ice::createProperties();
        props->setProperty("Ice.Plugin.IceSSL", "IceSSL:createIceSSL");
        props->setProperty("Ice.Override.Timeout", "5000");

        /* Increase the size of the client thread pool. Otherwise callbacks
         * won't work. The standard thread pool contains just 1 thread. If the
         * server uses a callback and the callback in turn tries to call a
         * function on the server in that callback the client will freeze
         * because it needs at least one thread per direction.
         *
         * see http://doc.zeroc.com/display/Ice/Nested+Invocations
         */
        props->setProperty("Ice.ThreadPool.Client.SizeMax", "5");

        if (opts & DontVerifyPeer)
            props->setProperty("IceSSL.VerifyPeer", "0");
        else
            props->setProperty("IceSSL.CertAuthFile", serverCertPath.toStdString());

        /* By default ICE will close connections if they are idle for more than
         * 60 seconds. This is bad because the server can only use the client callback
         * through the connection the client made to it.
         * For sessions this HAS to be disabled.
         *
         * see http://doc.zeroc.com/display/Ice/Bidirectional+Connections
         */
        props->setProperty("Ice.ACM.Client", "0");

        Ice::InitializationData id;
        id.properties = props;

        communicator = Ice::initialize(id);
        auth = sdc::AuthenticationIPrx::checkedCast(
                   communicator->stringToProxy(conn.toStdString()));

        if (!auth) {
            msg = "Invalid proxy";
            success = false;
        }
    }

    ~CommunicatorPtrWrapper() {
        if (!(opts & DontDestroyComm))
            communicator->destroy();
    }

    Ice::CommunicatorPtr communicator;
    sdc::AuthenticationIPrx auth;

private:
    Options opts;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CommunicatorPtrWrapper::Options)

/**
 * Needed to make this public unfortunately, save someone tells me
 * how to connect a signal from an instance you don't have access to...
 */
SessionManager *SessionManager::getInstance()
{
    return &instance;
}

void SessionManager::login(const QString &serverName, const QString &serverCertPath,
                           QSharedPointer<const LoginUser> usr, const QString &pwd)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QtConcurrent::run(&instance, &SessionManager::runLogin,
                      serverName, serverCertPath, usr, pwd);
}

void SessionManager::runLogin(const QString &serverName, const QString &serverCertPath,
                              QSharedPointer<const LoginUser> usr, const QString &pwd)
{
    bool success;
    QString msg;
    QSharedPointer<Session> session;

    try {
        CommunicatorPtrWrapper commWrapper(serverName, serverCertPath,
                                           CommunicatorPtrWrapper::DontDestroyComm,
                                           success, msg);
        if (!success)
            goto out;

        session = QSharedPointer<Session>(new Session(usr, pwd, commWrapper.auth));
    } catch (const sdc::SDCException &e) {
        msg = e.what.c_str();
        success = false;
    } catch (const Ice::Exception &e) {
        msg = e.what();
        success = false;
    } catch (...) {
        msg = "Unexpected exception";
        success = false;
    }

out:
    emit loginCompleted(session, success, msg);
}

void SessionManager::registerUser(const QString &serverName,
                                  const QString &serverCertPath,
                                  QSharedPointer<const User> usr, const QString &pwd)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QtConcurrent::run(&instance, &SessionManager::runRegisterUser,
                      serverName, serverCertPath, usr, pwd);
}

void SessionManager::runRegisterUser(const QString &serverName,
                                     const QString &serverCertPath,
                                     QSharedPointer<const User> usr, const QString &pwd)
{
    bool success;
    QString msg;

    try {
        CommunicatorPtrWrapper commWrapper(serverName, serverCertPath,
                                           CommunicatorPtrWrapper::NoOptions,
                                           success, msg);
        if (!success)
            goto out;

        QSharedPointer<sdc::User> usrPtr = usr->getIceUser();
        commWrapper.auth->registerUser(*(usrPtr.data()), pwd.toStdString());
    } catch (const sdc::SDCException &e) {
        msg = e.what.c_str();
        success = false;
    } catch (const Ice::Exception &e) {
        msg = e.what();
        success = false;
    } catch (...) {
        msg = "Unexpected exception";
        success = false;
    }

out:
    emit registerCompleted(success, msg);
}

void SessionManager::testConnection(const QString &serverName)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QtConcurrent::run(&instance, &SessionManager::runTestConnection,
                      serverName);
}

void SessionManager::runTestConnection(const QString &serverName)
{
    bool success = false;
    QString msg = "";

    try {
        CommunicatorPtrWrapper commWrapper(serverName, QString(),
                                           CommunicatorPtrWrapper::DontVerifyPeer,
                                           success, msg);
        if (!success)
            goto out;

        if (commWrapper.auth->echo("istenszerete") != "istenszerete") {
            success = false;
            msg = "Server gave wrong answer";
        }
    } catch (const sdc::SDCException &e) {
        msg = e.what.c_str();
        success = false;
    } catch (const Ice::Exception &e) {
        msg = e.what();
        success = false;
    } catch (...) {
        msg = "Unexpected exception";
        success = false;
    }

out:
    emit testConnectionCompleted(success, msg);
}

/** the instance must be defined somewhere */
SessionManager SessionManager::instance;

}

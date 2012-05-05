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

        if (opts & DontVerifyPeer)
            props->setProperty("IceSSL.VerifyPeer", "0");
        else
            props->setProperty("IceSSL.CertAuthFile", serverCertPath.toStdString());

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
                           const User &usr, const QString &pwd)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QtConcurrent::run(&instance, &SessionManager::runLogin,
                      serverName, serverCertPath, usr, pwd);
}

void SessionManager::runLogin(const QString &serverName, const QString &serverCertPath,
                              const User &usr, const QString &pwd)
{
    bool success;
    QString msg;
    Session *session;

    try {
        CommunicatorPtrWrapper commWrapper(serverName, serverCertPath,
                                           CommunicatorPtrWrapper::DontDestroyComm,
                                           success, msg);
        if (!success)
            goto out;

        session = new Session(usr, pwd, commWrapper.auth);
    } catch (const sdc::SDCException &e) {
        msg = e.what.c_str();
        success = false;
    } catch (const Ice::Exception &e) {
        msg = e.what();
        success = false;
    }

out:
    emit loginCompleted(QSharedPointer<Session>(session), success, msg);
}

void SessionManager::registerUser(const QString &serverName,
                                  const QString &serverCertPath,
                                  const User &usr, const QString &pwd)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QtConcurrent::run(&instance, &SessionManager::runRegisterUser,
                      serverName, serverCertPath, usr, pwd);
}

void SessionManager::runRegisterUser(const QString &serverName,
                                     const QString &serverCertPath,
                                     const User &usr, const QString &pwd)
{
    bool success;
    QString msg;

    try {
        CommunicatorPtrWrapper commWrapper(serverName, serverCertPath,
                                           CommunicatorPtrWrapper::NoOptions,
                                           success, msg);
        if (!success)
            goto out;

        QSharedPointer<sdc::User> usrPtr = usr.getIceUser();
        commWrapper.auth->registerUser(*(usrPtr.data()), pwd.toStdString());
    } catch (const sdc::SDCException &e) {
        msg = e.what.c_str();
        success = false;
    } catch (const Ice::Exception &e) {
        msg = e.what();
        success = false;
    }

out:
    emit registerCompleted(success, msg);
}

void SessionManager::testConnection (const QString &serverName)
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
    }

out:
    emit testConnectionCompleted (success, msg);
}

/** the instance must be defined somewhere */
SessionManager SessionManager::instance;

}

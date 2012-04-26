#include "sessionmanager.h"

#include <QtConcurrentRun>
#include <SecureDistributedChat.h>

namespace sdcc
{

/**
 * Needed to make this public unfortunately, save someone tells me
 * how to connect a signal from an instance you don't have access to...
 */
SessionManager *SessionManager::getInstance ()
{
    return &instance;
}

void SessionManager::testConnection (const QString &serverName,
                                     const QString &serverCertPath)
{

    QtConcurrent::run(&instance, &SessionManager::runTestConnection,
                      serverName, serverCertPath);
}

void SessionManager::runTestConnection(const QString &serverName,
                                       const QString &serverCertPath)
{
    bool success = false;
    QString msg = "";

    try {
        // We only use ssl!
        QString conn = QString("Authentication:ssl -h %1 -p %2")
                       .arg(serverName).arg(sdc::port);

        // This communicator is only temporary, we will destroy it at the end.
        Ice::CommunicatorPtr ic = makeCommunicator(serverCertPath);
        Ice::ObjectPrx base = ic->stringToProxy(conn.toStdString());
        sdc::AuthenticationIPrx auth = sdc::AuthenticationIPrx::checkedCast(base);

        if (!auth) {
            msg = "Invalid proxy";
            success = false;
        } else {
            success = (auth->echo("istenszerete") == "istenszerete");
            if (!success) {
                msg = "Server gave wrong answer";
            }
        }

        ic->destroy();

    } catch (const sdc::SDCException &e) {
        msg = e.what.c_str();
    } catch (const Ice::Exception &e) {
        // The message returned is not very meaningful, unfortunately.
        msg = e.what();
    }

    emit testConnectionCompleted (success, msg);
}

Ice::CommunicatorPtr SessionManager::makeCommunicator(const QString &serverCertPath)
{
    Ice::CommunicatorPtr ic;

    /* Set up ice to initialize ssl plugin and set used cert file. */
    Ice::PropertiesPtr props = Ice::createProperties();
    props->setProperty("Ice.Plugin.IceSSL", "IceSSL:createIceSSL");
    props->setProperty("IceSSL.CertAuthFile", serverCertPath.toStdString());
    props->setProperty("Ice.Override.Timeout", "5000");

    Ice::InitializationData id;
    id.properties = props;

    ic = Ice::initialize(id);

    return ic;
}

/** the instance must be defined somewhere */
SessionManager SessionManager::instance;

}

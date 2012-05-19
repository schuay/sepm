
#include <QCoreApplication>
#include <QSharedPointer>
#include <unistd.h>

#include "QsLog.h"
#include "QsLogDest.h"
#include <QtCrypto>

#include <Ice/Ice.h>
#include <iostream>
#include "SecureDistributedChat.h"

#include "userdbproxy.h"
#include "authentication.h"

namespace sdcs
{

static void usage()
{
    fprintf(stderr,
            "-d driver      set database driver (http://qt-project.org/doc/qt-4.8/sql-driver.html)\n"
            "-b database    set database name\n"
            "-h host        set database host\n"
            "-u user        set database user\n"
            "-p password    set database password\n");
}

struct Server : virtual public Ice::Application {

    /**
     * Run the server
     * Returns only when the server is being exited.
     */
    virtual int run(int argc, char *argv[]) {
        QLOG_TRACE() << __PRETTY_FUNCTION__;

        /* Parse command line arguments. They are stripped of any
        QT or ICE options by now. */

        int opt;
        while ((opt = getopt(argc, argv, "d:b:h:u:p:")) != -1) {
            switch (opt) {
            case 'd':
                sdcs::UserDbProxy::setDriver(optarg);
                break;
            case 'b':
                sdcs::UserDbProxy::setDatabase(optarg);
                break;
            case 'h':
                sdcs::UserDbProxy::setHost(optarg);
                break;
            case 'u':
                sdcs::UserDbProxy::setUser(optarg);
                break;
            case 'p':
                sdcs::UserDbProxy::setPassword(optarg);
                break;
            default:
                usage();
                return 1;
            }
        }

        if (optind != argc) {
            fprintf(stderr, "%d unparsed arguments\n", argc - optind);
            QLOG_WARN() << argc - optind << "unparsed arguments";
        }

        startObjectAdapter();

        communicator()->waitForShutdown();

        return 0;
    }

    /**
     * Starts the object adapter. There need only be a single adapter for all
     * server tasks. No reference is held after this function, if one is needed,
     * this should be changed to store it in a member.
     */
    void startObjectAdapter() {
        QLOG_TRACE() << __PRETTY_FUNCTION__;

        /* Note that while the name of the object adapter is not important,
        the name of the Ice Identity matters - it is the name of the implemented
         interface bar the trailing 'I' */
        std::ostringstream oss;
        oss << "ssl -p " << sdc::port;
        Ice::ObjectAdapterPtr adapter =
            communicator()->createObjectAdapterWithEndpoints("SDCServer", oss.str());
        Ice::ObjectPtr authObj = new Authentication;

        adapter->add(authObj, communicator()->stringToIdentity("Authentication"));
        adapter->activate();
    }

};

}

int main(int argc, char **argv)
{
    /* Temporary debug code. */
    /*    QSharedPointer<sdcs::UserDbProxy> p = sdcs::UserDbProxy::getProxy("jakob");
      printf("%s\n", p->getUser().ID.c_str());*/

    QCoreApplication a(argc, argv);

    /* Init QSettings */
    QCoreApplication::setOrganizationName("group22");
    QCoreApplication::setApplicationName("sdcs");

    /* Init logger */
    QsLogging::Logger &logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);

    QsLogging::DestinationPtr debugDestination(
        QsLogging::DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination.get());

    QCA::init();

    // Set Ice properties here
    // they should be read from settings actually
    Ice::PropertiesPtr props = Ice::createProperties();
    props->setProperty("Ice.Plugin.IceSSL", "IceSSL:createIceSSL");
    props->setProperty("Ice.Override.Timeout", "5000");
    props->setProperty("IceSSL.DefaultDir", "misc");
    props->setProperty("IceSSL.CertFile", "localhost.crt");
    props->setProperty("IceSSL.KeyFile", "localhost.key");
    props->setProperty("IceSSL.CertAuthFile", "ca.crt");
    props->setProperty("IceSSL.VerifyPeer", "0");

    Ice::InitializationData id;
    id.properties = props;

    sdcs::Server srv;

    return srv.main(argc, argv, id);
}

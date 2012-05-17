
#include <QCoreApplication>
#include <QSharedPointer>
#include <unistd.h>

#include "QsLog.h"
#include "QsLogDest.h"

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

    virtual int run(int argc, char* argv[])
    {

        QCoreApplication a(argc, argv);

        /* Init QSettings */
        QCoreApplication::setOrganizationName("group22");
        QCoreApplication::setApplicationName("sdcs");

        /* Init logger */
        QsLogging::Logger& logger = QsLogging::Logger::instance();
        logger.setLoggingLevel(QsLogging::TraceLevel);

        QsLogging::DestinationPtr debugDestination(
            QsLogging::DestinationFactory::MakeDebugOutputDestination());
        logger.addDestination(debugDestination.get());



        QLOG_TRACE() << __PRETTY_FUNCTION__;

        /* Parse command line arguments. */

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
        std::cout <<"Ice Server started" << std::endl;

        startObjectAdapter();

        communicator()->waitForShutdown();

        return 0;
    }

    void startObjectAdapter()
    {


        Ice::ObjectAdapterPtr adapter =
            communicator()->createObjectAdapterWithEndpoints("Authentication", "authentication:ssl -p 1337");
        Ice::ObjectPtr authObj = new Authentication;
        adapter->add(authObj, communicator()->stringToIdentity("AuthenticationI"));
        adapter->activate();

    }

};

}

int main(int argc, char **argv)
{
    /* Temporary debug code. */
    /*    QSharedPointer<sdcs::UserDbProxy> p = sdcs::UserDbProxy::getProxy("jakob");
      printf("%s\n", p->getUser().ID.c_str());*/

    // Set Ice properties here
    Ice::PropertiesPtr props = Ice::createProperties();
    props->setProperty("Ice.Plugin.IceSSL", "IceSSL:createIceSSL");
    props->setProperty("Ice.Override.Timeout", "5000");

    Ice::InitializationData id;
    id.properties = props;

    sdcs::Server srv;

    return srv.main(argc, argv, id);
}

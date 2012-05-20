
#include <QCoreApplication>
#include <QtCrypto>
#include <unistd.h>
#include <Ice/Ice.h>

#include "QsLog.h"
#include "QsLogDest.h"
#include "authentication.h"
#include "userdbproxy.h"
#include "server.h"

static void usage()
{
    fprintf(stderr,
            "-d driver      set database driver (http://qt-project.org/doc/qt-4.8/sql-driver.html)\n"
            "-b database    set database name\n"
            "-h host        set database host\n"
            "-u user        set database user\n"
            "-p password    set database password\n");
}

struct Application : virtual public Ice::Application {

    /**
     * Run the server
     * Returns only when the server is being exited.
     */
    virtual int run(int argc, char *argv[]) {
        QLOG_TRACE() << __PRETTY_FUNCTION__;

        /* Parse command line arguments. They are stripped of any
         * QT or ICE options by now. */

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

        /* The server handles adapter setup and wraps needed interfaces. */
        sdcs::Server server(communicator());
        Q_UNUSED(server);

        communicator()->waitForShutdown();

        return 0;
    }
};

int main(int argc, char **argv)
{
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

    /* Init Ice properties. */
    Ice::PropertiesPtr props = Ice::createProperties();
    props->setProperty("Ice.Plugin.IceSSL", "IceSSL:createIceSSL");
    props->setProperty("Ice.Override.Timeout", "5000");
    props->setProperty("IceSSL.DefaultDir", "misc"); /* TODO: a better way to set this */
    props->setProperty("IceSSL.CertFile", "localhost.crt");
    props->setProperty("IceSSL.KeyFile", "localhost.key");
    props->setProperty("IceSSL.CertAuthFile", "ca.crt");
    props->setProperty("IceSSL.VerifyPeer", "0");

    /* Again, nested calls lead to deadlock issues. This time, the problem
     * was caused by the echo() call in Authentication::login(). */
    props->setProperty("Ice.ThreadPool.Client.SizeMax", "5");
    props->setProperty("Ice.ThreadPool.Server.SizeMax", "5");

    Ice::InitializationData id;
    id.properties = props;

    /* Handles initialization and cleanup of QCA resources. */
    QCA::Initializer qca;
    Q_UNUSED(qca);

    /* Our application handles the main loop. */
    Application srv;

    return srv.main(argc, argv, id);
}

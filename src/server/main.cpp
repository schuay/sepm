#include <QCoreApplication>
#include <QSharedPointer>
#include <unistd.h>

#include "QsLog.h"
#include "QsLogDest.h"
#include "userdbproxy.h"

static void usage()
{
    fprintf(stderr,
            "-d driver      set database driver (http://qt-project.org/doc/qt-4.8/sql-driver.html)\n"
            "-b database    set database name\n"
            "-h host        set database host\n"
            "-u user        set database user\n"
            "-p password    set database password\n");
}

int main(int argc, char **argv)
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

    /* Temporary debug code. */

    QSharedPointer<sdcs::UserDbProxy> p = sdcs::UserDbProxy::getProxy("jakob");
    printf("%s\n", p->getUser().ID.c_str());

    return 0;
}

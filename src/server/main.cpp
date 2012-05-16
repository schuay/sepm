#include <QApplication>
#include <QSharedPointer>

#include "QsLog.h"
#include "QsLogDest.h"
#include "userdbproxy.h"


int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    /* Init QSettings */
    QCoreApplication::setOrganizationName("group22");
    QCoreApplication::setApplicationName("sdcs");

    /* Init logger */
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);

    QsLogging::DestinationPtr debugDestination(
        QsLogging::DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination.get());

    sdcs::UserDbProxy::setDatabase("jakob");
    sdcs::UserDbProxy::setHost("localhost");
    sdcs::UserDbProxy::setUser("jakob");

    QSharedPointer<sdcs::UserDbProxy> p = sdcs::UserDbProxy::getProxy("jakob");



    return 0;
    // return a.exec();
}

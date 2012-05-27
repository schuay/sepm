#include <QApplication>

#include "logindialog.h"
#include "QsLog.h"
#include "QsLogDest.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    /* Init QSettings */
    QCoreApplication::setOrganizationName("group22");
    QCoreApplication::setApplicationName("sdc");

    sdc::Settings::init();

    /* Init logger */
    QsLogging::Logger &logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);

    QsLogging::DestinationPtr debugDestination(
        QsLogging::DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination.get());

    LoginDialog *win = new LoginDialog();
    win->show();

    return a.exec();
}

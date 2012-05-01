#include <QApplication>

#include "loginwindow.h"
#include "QsLog.h"
#include "QsLogDest.h"

using namespace sdcc;

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(client_gui_qml);

    /* Init logger */
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);

    QsLogging::DestinationPtr debugDestination(
        QsLogging::DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination.get());

    LoginWindow win;
    win.show();
    return a.exec();
}

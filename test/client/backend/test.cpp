#include "test.h"

#include "sessionmanager.h"

using namespace sdcc;

/**
 * Just a little tester for anything in the backend. Feel free to modify
 * and destroy it however you like to...
 **/
int main (int argc , char **argv)
{

    QApplication app (argc, argv);
    test reporter;

    QObject::connect(SessionManager::getInstance(),
                     SIGNAL(testConnectionCompleted(bool, const QString&)),
                     &reporter, SLOT(report(bool, const QString&)));

    SessionManager::testConnection("selinux.inso.tuwien.ac.at", "ca.crt");

    printf("call returned\n");
    app.exec();

}

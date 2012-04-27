#include "test.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include "sessionmanager.h"

using namespace sdcc;

QTEST_MAIN(ClientBackendTests)

void ClientBackendTests::testTestConnection()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(testConnectionCompleted(bool, const QString &)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    sessionManager->testConnection("selinux.inso.tuwien.ac.at", "ca.crt");

    /* We need to actually wait for the signal to be emitted.
       Googling turns up Q_TRY_VERIFY, but it seems that this is only
       included in Qt5. Alternatively, just wait for some random amount (250ms).
     */

    /* Wait for result. */
    QTest::qWait(250);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0) == true);
}

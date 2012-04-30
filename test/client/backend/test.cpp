#include "test.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include <QCryptographicHash>

#include "sessionmanager.h"
#include "user.h"
#include <Security.h>

using namespace sdcc;

QTEST_MAIN(ClientBackendTests)

/**
 * Checks every 50 ms if spy has received a result, and returns if it did.
 * Otherwise, it aborts after 10 seconds.
 */
static void waitForResult(const QSignalSpy &spy)
{
    for (int i = 0; i < 200; i++) {
        QTest::qWait(50);
        if (spy.count() > 0)
            break;
    }
}

void ClientBackendTests::testTestConnection()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(testConnectionCompleted(bool, const QString &)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    sessionManager->testConnection("selinux.inso.tuwien.ac.at");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == true, arguments.at(1).toString().toStdString().c_str());
}

void ClientBackendTests::testUserCtorNamePath()
{
    try {
        User u("user@example.com", "public.pem");
        QCOMPARE(u.getName(), QString("user@example.com"));
    } catch (sdc::SecurityException s) {
        QFAIL(s.what());
    }
}

void ClientBackendTests::testUserCtorSdcUser()
{
    sdc::Security s;
    sdc::ByteSeq pubkey = s.readPubKey("public.pem");
    sdc::User su = { "user@example.com", pubkey };
    User u(su);

    QCOMPARE(u.getName(), QString("user@example.com"));
}

void ClientBackendTests::testUserGetIceUser()
{
    sdc::Security s;
    sdc::ByteSeq pubkey = s.readPubKey("public.pem");
    sdc::User su1 = { "user@example.com", pubkey };

    User u(su1);
    QSharedPointer<sdc::User> ptr = u.getIceUser();
    sdc::User *su2 = ptr.data();

    QCOMPARE(su1, *su2);
}

void ClientBackendTests::testRegisterUserNew()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(registerCompleted(bool, const QString &)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QString randomName = QString(QCryptographicHash::hash(
                                     QTime::currentTime().toString(
                                         "hh:mm:ss.zzz").toAscii(),
                                     QCryptographicHash::Sha1).toHex());
    User u(QString("%1@selinux.inso.tuwien.ac.at").arg(randomName.left(10)),
           "public.pem");
    sessionManager->registerUser("selinux.inso.tuwien.ac.at", "ca.crt", u,
                                 "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == true, arguments.at(1).toString().toStdString().c_str());
}

void ClientBackendTests::testRegisterUserAgain()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(registerCompleted(bool, const QString &)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("fefeb10c@selinux.inso.tuwien.ac.at", "public.pem");
    sessionManager->registerUser("selinux.inso.tuwien.ac.at", "ca.crt", u,
                                 "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0) == false);
}

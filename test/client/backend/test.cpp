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
static void waitForResult(const QSignalSpy &spy, int count = 1)
{
    for (int i = 0; i < 200; i++) {
        QTest::qWait(50);
        if (spy.count() >= count)
            break;
    }
}

Q_DECLARE_METATYPE(QSharedPointer<Session>)
void ClientBackendTests::initTestCase()
{
    qRegisterMetaType<QSharedPointer<Session> >("QSharedPointer<Session>");
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

void ClientBackendTests::testLoginNonexistentUser()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("thisuserbetternotexist@selinux.inso.tuwien.ac.at", "public.pem");
    sessionManager->login("selinux.inso.tuwien.ac.at", "ca.crt", u,
                          "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(1) == false);
}

void ClientBackendTests::testLoginIncorrectCredentials()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("fefeb10c@selinux.inso.tuwien.ac.at", "public.pem");
    sessionManager->login("selinux.inso.tuwien.ac.at", "ca.crt", u,
                          "wrongpassword");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(1) == false);
}

void ClientBackendTests::testLoginCorrectCredentials()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("fefeb10c@selinux.inso.tuwien.ac.at", "public.pem");
    sessionManager->login("selinux.inso.tuwien.ac.at", "ca.crt", u,
                          "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());
}

void ClientBackendTests::testLoginRepeated()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("fefeb10c@selinux.inso.tuwien.ac.at", "public.pem");
    sessionManager->login("selinux.inso.tuwien.ac.at", "ca.crt", u,
                          "password");
    sessionManager->login("selinux.inso.tuwien.ac.at", "ca.crt", u,
                          "password");

    waitForResult(spy, 2);

    QCOMPARE(spy.count(), 2);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());
    arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());
}

/* TODO: refactor into separate class with init() doing the session setup. */
void ClientBackendTests::testLogout()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("fefeb10c@selinux.inso.tuwien.ac.at", "public.pem");
    sessionManager->login("selinux.inso.tuwien.ac.at", "ca.crt", u,
                          "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());

    QSharedPointer<Session> session = arguments.at(0).value<QSharedPointer<Session> >();
    QSignalSpy sessionSpy(session.data(),
                          SIGNAL(logoutCompleted(bool, QString)));

    QVERIFY(sessionSpy.isValid());
    QVERIFY(sessionSpy.isEmpty());

    session->logout();
    waitForResult(sessionSpy);

    QCOMPARE(sessionSpy.count(), 1);
    arguments = sessionSpy.takeFirst();
    QVERIFY2(arguments.at(0) == true, arguments.at(1).toString().toStdString().c_str());
}

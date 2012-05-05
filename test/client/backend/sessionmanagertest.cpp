#include "sessionmanagertest.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include <QCryptographicHash>

#include "sessionmanager.h"
#include "user.h"
#include "Security.h"
#include "common.h"

using namespace sdcc;

QTEST_MAIN(SessionManagerTests)

Q_DECLARE_METATYPE(QSharedPointer<Session>)
void SessionManagerTests::initTestCase()
{
    qRegisterMetaType<QSharedPointer<Session> >("QSharedPointer<Session>");
}

void SessionManagerTests::testTestConnection()
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

void SessionManagerTests::testUserCtorNamePath()
{
    try {
        User u("user@example.com", WORKING_DIR "public.pem");
        QCOMPARE(u.getName(), QString("user@example.com"));
    } catch (sdc::SecurityException s) {
        QFAIL(s.what());
    }
}

void SessionManagerTests::testUserCtorSdcUser()
{
    sdc::Security s;
    sdc::ByteSeq pubkey = s.readPubKey(WORKING_DIR "public.pem");
    sdc::User su = { "user@example.com", pubkey };
    User u(su);

    QCOMPARE(u.getName(), QString("user@example.com"));
}

void SessionManagerTests::testUserGetIceUser()
{
    sdc::Security s;
    sdc::ByteSeq pubkey = s.readPubKey(WORKING_DIR "public.pem");
    sdc::User su1 = { "user@example.com", pubkey };

    User u(su1);
    QSharedPointer<sdc::User> ptr = u.getIceUser();
    sdc::User *su2 = ptr.data();

    QCOMPARE(su1, *su2);
}

void SessionManagerTests::testRegisterUserNew()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(registerCompleted(bool, const QString &)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u(TEMP_SESSION_USER, WORKING_DIR "public.pem");
    sessionManager->registerUser("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u,
                                 "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == true, arguments.at(1).toString().toStdString().c_str());
}

void SessionManagerTests::testRegisterUserRandom()
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
           WORKING_DIR "public.pem");
    sessionManager->registerUser("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u,
                                 "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == true, arguments.at(1).toString().toStdString().c_str());
}

void SessionManagerTests::testRegisterUserAgain()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(registerCompleted(bool, const QString &)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("fefeb10c@selinux.inso.tuwien.ac.at", WORKING_DIR "public.pem");
    sessionManager->registerUser("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u,
                                 "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0) == false);
}

void SessionManagerTests::testLoginNonexistentUser()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("thisuserbetternotexist@selinux.inso.tuwien.ac.at", WORKING_DIR "public.pem");
    sessionManager->login("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u,
                          "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(1) == false);
}

void SessionManagerTests::testLoginIncorrectCredentials()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("fefeb10c@selinux.inso.tuwien.ac.at", WORKING_DIR "public.pem");
    sessionManager->login("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u,
                          "wrongpassword");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(1) == false);
}

void SessionManagerTests::testLoginCorrectCredentials()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("fefeb10c@selinux.inso.tuwien.ac.at", WORKING_DIR "public.pem");
    sessionManager->login("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u,
                          "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());
}

void SessionManagerTests::testLoginRepeated()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("fefeb10c@selinux.inso.tuwien.ac.at", WORKING_DIR "public.pem");
    sessionManager->login("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u,
                          "password");
    sessionManager->login("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u,
                          "password");

    waitForResult(spy, 2);

    QCOMPARE(spy.count(), 2);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());
    arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());
}

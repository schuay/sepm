#include "sessiontest.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include <QCryptographicHash>

#include "sessionmanager.h"
#include "user.h"
#include "common.h"

QTEST_MAIN(SessionTests)


Q_DECLARE_METATYPE(QSharedPointer<Session>)
Q_DECLARE_METATYPE(QSharedPointer<User>)
void SessionTests::initTestCase()
{
    qRegisterMetaType<QSharedPointer<Session> >("QSharedPointer<Session>");
    qRegisterMetaType<QSharedPointer<Chat> >("QSharedPointer<Chat>");
    qRegisterMetaType<QSharedPointer<User> >("QSharedPointer<User>");
}

void SessionTests::init()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    LoginUser u(TEMP_SESSION_USER, WORKING_DIR "public.pem", WORKING_DIR "private.pem");
    sessionManager->login("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u,
                          "password");

    waitForResult(spy);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());

    session = arguments.at(0).value<QSharedPointer<Session> >();
}

void SessionTests::testInitChat()
{
    QSignalSpy spy(session.data(), SIGNAL(initChatCompleted(QSharedPointer<Chat>, bool,
                                          QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->initChat();
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());
}

void SessionTests::testLogout()
{
    QSignalSpy spy(session.data(), SIGNAL(logoutCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->logout();
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == true, arguments.at(1).toString().toStdString().c_str());
}

void SessionTests::testDeleteUserNonexistent()
{
    QSignalSpy spy(session.data(), SIGNAL(deleteUserCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->deleteUser(User("thisuserbetternotexist@selinux.inso.tuwien.ac.at", WORKING_DIR "public.pem"));
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == false, arguments.at(1).toString().toStdString().c_str());
}

void SessionTests::testDeleteUser()
{
    QSignalSpy spy(session.data(), SIGNAL(deleteUserCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->deleteUser(User(TEMP_SESSION_USER, WORKING_DIR "public.pem"));
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == true, arguments.at(1).toString().toStdString().c_str());
}

void SessionTests::testDeleteUserUnauthorized()
{
    QSignalSpy spy(session.data(), SIGNAL(deleteUserCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->deleteUser(User("fefeb10c@selinux.inso.tuwien.ac.at", WORKING_DIR "public.pem"));
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0) == false);
}

void SessionTests::retrieveUser()
{
    QSignalSpy spy(session.data(), SIGNAL(retrieveUserCompleted(QSharedPointer<User> ,bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->retrieveUser("fefeb10c@selinux.inso.tuwien.ac.at");
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());

    QSharedPointer<User> usr = arguments.at(0).value<QSharedPointer<User> >();
    QCOMPARE(usr.data()->getName(), QString("fefeb10c@selinux.inso.tuwien.ac.at"));
}

void SessionTests::retrieveUserNonexistent()
{
    QSignalSpy spy(session.data(), SIGNAL(retrieveUserCompleted(QSharedPointer<User> ,bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->retrieveUser("thisuserbetternotexist@selinux.inso.tuwien.ac.at");
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == false, arguments.at(2).toString().toStdString().c_str());
}

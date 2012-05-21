#include "sessiontest.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include <QCryptographicHash>

#include "sessionmanager.h"
#include "user.h"
#include "common.h"

QTEST_MAIN(SessionTests)


Q_DECLARE_METATYPE(QSharedPointer<Session>)
Q_DECLARE_METATYPE(QSharedPointer<const User>)
void SessionTests::initTestCase()
{
    qRegisterMetaType<QSharedPointer<Session> >("QSharedPointer<Session>");
    qRegisterMetaType<QSharedPointer<Chat> >("QSharedPointer<Chat>");
    qRegisterMetaType<QSharedPointer<const User> >("QSharedPointer<const User>");
    qRegisterMetaType<const QObject *>("const QObject*");
}

void SessionTests::init()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>, bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const LoginUser> u(new LoginUser(TEMP_SESSION_USER, WORKING_DIR "public.pem",
                                      WORKING_DIR "private.pem"));
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

    session->deleteUser(QSharedPointer<const User>(new User(
                            "thisuserbetternotexist@selinux.inso.tuwien.ac.at",
                            WORKING_DIR "public.pem")));
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

    session->deleteUser(QSharedPointer<const User>(new User(TEMP_SESSION_USER,
                        WORKING_DIR "public.pem")));
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

    session->deleteUser(QSharedPointer<const User>(new User("fefeb10c@selinux.inso.tuwien.ac.at",
                        WORKING_DIR "public.pem")));
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0) == false);
}

void SessionTests::retrieveUser()
{
    QSignalSpy spy(session.data(), SIGNAL(retrieveUserCompleted(QSharedPointer<const User>,
                                          const QObject *, bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->retrieveUser("fefeb10c@selinux.inso.tuwien.ac.at", this);
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(2) == true, arguments.at(3).toString().toStdString().c_str());

    QSharedPointer<const User> usr = arguments.at(0).value<QSharedPointer<const User> >();
    QCOMPARE(usr.data()->getName(), QString("fefeb10c@selinux.inso.tuwien.ac.at"));
}

void SessionTests::retrieveUserNonexistent()
{
    QSignalSpy spy(session.data(), SIGNAL(retrieveUserCompleted(QSharedPointer<const User>,
                                          const QObject *, bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->retrieveUser("thisuserbetternotexist@selinux.inso.tuwien.ac.at", this);
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(2) == false, arguments.at(3).toString().toStdString().c_str());
}

void SessionTests::retrieveContactListNonexistent()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>, bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const LoginUser> u(new LoginUser("fefeb10c@selinux.inso.tuwien.ac.at",
                                      WORKING_DIR "public.pem", WORKING_DIR "private.pem"));
    sessionManager->login("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u,
                          "password");

    waitForResult(spy);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());

    session = arguments.at(0).value<QSharedPointer<Session> >();

    QSignalSpy spy2(session.data(), SIGNAL(retrieveContactListCompleted(const QStringList,
                                           bool, const QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    session->retrieveContactList();
    waitForResult(spy2);

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> arguments2 = spy2.takeFirst();
    QStringList contactsRet = arguments2.at(0).value<QStringList>();

    QVERIFY(arguments2.at(1) == false);
}

void SessionTests::saveRetrieveContactList()
{
    QStringList contacts = QString("pinkie_pie@selinux.inso.tuwien.ac.at\n"
                                   "rainbow_dash@selinux.inso.tuwien.ac.at").split('\n');

    QSignalSpy spy(session.data(), SIGNAL(saveContactListCompleted(bool, const QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->saveContactList(contacts);
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == true, arguments.at(1).toString().toStdString().c_str());

    QSignalSpy spy2(session.data(), SIGNAL(retrieveContactListCompleted(const QStringList,
                                           bool, const QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    session->retrieveContactList();
    waitForResult(spy2);

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> arguments2 = spy2.takeFirst();
    QStringList contactsRet = arguments2.at(0).value<QStringList>();

    QVERIFY2(arguments2.at(1) == true, arguments2.at(2).toString().toStdString().c_str());
    QVERIFY(contacts == contactsRet);
}

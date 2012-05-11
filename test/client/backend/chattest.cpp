#include "chattest.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include <QCryptographicHash>

#include "sessionmanager.h"
#include "user.h"
#include "common.h"

QTEST_MAIN(ChatTests)


Q_DECLARE_METATYPE(QSharedPointer<Session>)
Q_DECLARE_METATYPE(QSharedPointer<Chat>)
void ChatTests::initTestCase()
{
    qRegisterMetaType<QSharedPointer<Session> >("QSharedPointer<Session>");
    qRegisterMetaType<QSharedPointer<Chat> >("QSharedPointer<Chat>");
}

void ChatTests::init()
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

    session = arguments.at(0).value<QSharedPointer<Session> >();

    QSignalSpy spy2(session.data(), SIGNAL(initChatCompleted(QSharedPointer<Chat>, bool,
                                           QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    session->initChat();
    waitForResult(spy2);

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> arguments2 = spy2.takeFirst();
    QVERIFY2(arguments2.at(1) == true, arguments2.at(2).toString().toStdString().c_str());

    chat = arguments2.at(0).value<QSharedPointer<Chat> >();

    QSignalSpy spy3(sessionManager,
                    SIGNAL(registerCompleted(bool, const QString &)));
    QVERIFY(spy3.isValid());
    QVERIFY(spy3.isEmpty());

    User u2("pinkie_pie@selinux.inso.tuwien.ac.at", WORKING_DIR "public.pem");
    sessionManager->registerUser("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u2,
                                 "password");

    waitForResult(spy3);

    QCOMPARE(spy3.count(), 1);
    QList<QVariant> arguments3 = spy3.takeFirst();
    QVERIFY(arguments3.at(0) == false);
}

void ChatTests::testInviteNotLoggedIn()
{
    QSignalSpy spy(chat.data(), SIGNAL(inviteCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("pinkie_pie@selinux.inso.tuwien.ac.at", WORKING_DIR "public.pem");
    chat->invite(u);
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == false, arguments.at(1).toString().toStdString().c_str());
}

void ChatTests::testInviteNonexistent()
{
    QSignalSpy spy(chat.data(), SIGNAL(inviteCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("thisuserbetternotexist@selinux.inso.tuwien.ac.at", WORKING_DIR "public.pem");
    chat->invite(u);
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == false, arguments.at(1).toString().toStdString().c_str());
}

void ChatTests::testInvite()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    User u("pinkie_pie@selinux.inso.tuwien.ac.at", WORKING_DIR "public.pem");
    sessionManager->login("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u,
                          "password");

    waitForResult(spy);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());

    QSharedPointer<Session> tempSession = arguments.at(0).value<QSharedPointer<Session> >();

    QSignalSpy spy2(chat.data(), SIGNAL(inviteCompleted(bool, QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    chat->invite(u);
    waitForResult(spy2);

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> arguments2 = spy2.takeFirst();
    QVERIFY2(arguments2.at(0) == true, arguments2.at(1).toString().toStdString().c_str());
}

void ChatTests::testEchoMessage()
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

    session = arguments.at(0).value<QSharedPointer<Session> >();

    QSignalSpy spy2(session.data(), SIGNAL(initChatCompleted(QSharedPointer<Chat>, bool,
                                           QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    session->initChat();
    waitForResult(spy2);

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> arguments2 = spy2.takeFirst();
    QVERIFY2(arguments2.at(1) == true, arguments2.at(2).toString().toStdString().c_str());

    chat = arguments2.at(0).value<QSharedPointer<Chat> >();

    QSignalSpy spy3(&(*chat),
                    SIGNAL(messageReceived(const User &, const QString &)));
    QVERIFY(spy3.isValid());
    QVERIFY(spy3.isEmpty());

    QString msg = "send this back to me!";
    chat->send(msg);

    waitForResult(spy3);

    QCOMPARE(spy3.count(), 1);
    QList<QVariant> arguments3 = spy3.takeFirst();
    QVERIFY(arguments3.at(1) == msg);
}

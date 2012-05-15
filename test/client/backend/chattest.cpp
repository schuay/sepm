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
    qRegisterMetaType<User>("User");
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

    QSharedPointer<Session> session2 = arguments.at(0).value<QSharedPointer<Session> >();

    QSignalSpy spy2(chat.data(), SIGNAL(inviteCompleted(bool, QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    QSignalSpy spy3(session2.data(), SIGNAL(invitationReceived(QSharedPointer<Chat>)));
    QVERIFY(spy3.isValid());
    QVERIFY(spy3.isEmpty());

    QSignalSpy spy4(chat.data(), SIGNAL(userJoined(const User&)));
    QVERIFY(spy4.isValid());
    QVERIFY(spy4.isEmpty());

    chat->invite(u);
    waitForResult(spy2);

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> arguments2 = spy2.takeFirst();
    QVERIFY2(arguments2.at(0) == true, arguments2.at(1).toString().toStdString().c_str());

    waitForResult(spy3);

    QCOMPARE(spy3.count(), 1);
    QList<QVariant> arguments3 = spy3.takeFirst();
    QSharedPointer<Chat> chat2 = arguments3.at(0).value<QSharedPointer<Chat> >();

    waitForResult(spy4);

    QCOMPARE(spy4.count(), 1);

    QList<QSharedPointer<User> > chat1Usr = chat->getUserList();
    QList<QSharedPointer<User> > chat2Usr = chat2->getUserList();

    QCOMPARE(chat1Usr.size(), 2);
    QCOMPARE(chat2Usr.size(), 2);

    QCOMPARE(chat1Usr[0]->getName(), QString("fefeb10c@selinux.inso.tuwien.ac.at"));
    QCOMPARE(chat1Usr[1]->getName(), QString("pinkie_pie@selinux.inso.tuwien.ac.at"));

    QCOMPARE(chat2Usr[0]->getName(), QString("fefeb10c@selinux.inso.tuwien.ac.at"));
    QCOMPARE(chat2Usr[1]->getName(), QString("pinkie_pie@selinux.inso.tuwien.ac.at"));
}

void ChatTests::testEchoMessage()
{
    QSignalSpy spy(chat.data(),
                   SIGNAL(messageReceived(const User &, const QString &)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QString msg = "send this back to me!";
    chat->send(msg);

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(1) == msg);
}

#include "chattest.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include <QCryptographicHash>

#include "sessionmanager.h"
#include "user.h"
#include "loginuser.h"
#include "common.h"

QTEST_MAIN(ChatTests)


Q_DECLARE_METATYPE(QSharedPointer<Session>)
Q_DECLARE_METATYPE(QSharedPointer<Chat>)
Q_DECLARE_METATYPE(QSharedPointer<const User>)
Q_DECLARE_METATYPE(User)
void ChatTests::initTestCase()
{
    qRegisterMetaType<QSharedPointer<Session> >("QSharedPointer<Session>");
    qRegisterMetaType<QSharedPointer<Chat> >("QSharedPointer<Chat>");
    qRegisterMetaType<QSharedPointer<const User> >("QSharedPointer<const User>");
    qRegisterMetaType<User>("User");
}

void ChatTests::init()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
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
                    SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)));
    QVERIFY(spy3.isValid());
    QVERIFY(spy3.isEmpty());

    QSharedPointer<const LoginUser> u2(new LoginUser("pinkie_pie@selinux.inso.tuwien.ac.at",
                                       WORKING_DIR "public.pem", WORKING_DIR "private.pem"));
    sessionManager->login("selinux.inso.tuwien.ac.at", WORKING_DIR "ca.crt", u2,
                          "password");

    waitForResult(spy3);
    QCOMPARE(spy3.count(), 1);
    QList<QVariant> arguments3 = spy3.takeFirst();
    QVERIFY2(arguments3.at(1) == true, arguments3.at(2).toString().toStdString().c_str());

    session2 = arguments3.at(0).value<QSharedPointer<Session> >();
}

void ChatTests::testInviteNotLoggedIn()
{
    QSignalSpy spy(chat.data(), SIGNAL(inviteCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const User> u(new User("registeredbutneverlogsin@selinux.inso.tuwien.ac.at",
                                          WORKING_DIR "public.pem"));
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

    QSharedPointer<const User> u(new User("thisuserbetternotexist@selinux.inso.tuwien.ac.at",
                                          WORKING_DIR "public.pem"));
    chat->invite(u);
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == false, arguments.at(1).toString().toStdString().c_str());
}

void ChatTests::testInvite()
{
    QSignalSpy spy2(chat.data(), SIGNAL(inviteCompleted(bool, QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    QSignalSpy spy3(session2.data(), SIGNAL(invitationReceived(QSharedPointer<Chat>)));
    QVERIFY(spy3.isValid());
    QVERIFY(spy3.isEmpty());

    QSignalSpy spy4(chat.data(), SIGNAL(userJoined(QSharedPointer<const User>)));
    QVERIFY(spy4.isValid());
    QVERIFY(spy4.isEmpty());

    QSharedPointer<const User> u(new User("pinkie_pie@selinux.inso.tuwien.ac.at",
                                          WORKING_DIR "public.pem"));
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
    QList<QVariant> arguments4 = spy4.takeFirst();
    QSharedPointer<const User> u2 = arguments4.at(0).value<QSharedPointer<const User> >();
    QCOMPARE(u2->getName(), QString("pinkie_pie@selinux.inso.tuwien.ac.at"));

    QList<QSharedPointer<const User> > chat1Usr = chat->getUserList();
    QList<QSharedPointer<const User> > chat2Usr = chat2->getUserList();

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
                   SIGNAL(messageReceived(QSharedPointer<const User>, const QString &)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QString msg = "send this back to me!";
    chat->send(msg);

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(1) == msg);
}

void ChatTests::testTransmitMessage()
{
    QSignalSpy spy(session2.data(), SIGNAL(invitationReceived(QSharedPointer<Chat>)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSignalSpy spy2(chat.data(), SIGNAL(userJoined(QSharedPointer<const User>)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    QSharedPointer<const User> u(new User("pinkie_pie@selinux.inso.tuwien.ac.at",
                                          WORKING_DIR "public.pem"));
    chat->invite(u);
    waitForResult(spy);

    QList<QVariant> arguments = spy.takeFirst();
    QSharedPointer<Chat> chat2 = arguments.at(0).value<QSharedPointer<Chat> >();

    waitForResult(spy2);

    QSignalSpy spy3(chat2.data(), SIGNAL(sendCompleted(bool, const QString&)));
    QVERIFY(spy3.isValid());
    QVERIFY(spy3.isEmpty());

    QSignalSpy spy4(chat.data(), SIGNAL(messageReceived(QSharedPointer<const User>, const QString&)));
    QVERIFY(spy4.isValid());
    QVERIFY(spy4.isEmpty());

    QSignalSpy spy5(chat2.data(), SIGNAL(messageReceived(QSharedPointer<const User>, const QString&)));
    QVERIFY(spy5.isValid());
    QVERIFY(spy5.isEmpty());

    chat2->send("Cupcakes!!!");

    waitForResult(spy3);

    QList<QVariant> arguments2 = spy3.takeFirst();
    QVERIFY(arguments2.at(0) == true);

    waitForResult(spy4);

    QList<QVariant> arguments3 = spy4.takeFirst();
    QCOMPARE(arguments3.at(1).toString(), QString("Cupcakes!!!"));
    QSharedPointer<const User> u2 = arguments3.at(0).value<QSharedPointer<const User> >();
    QCOMPARE(u2->getName(), u->getName());

    waitForResult(spy5);

    QList<QVariant> arguments4 = spy5.takeFirst();
    QCOMPARE(arguments4.at(1).toString(), QString("Cupcakes!!!"));
    QSharedPointer<const User> u3 = arguments4.at(0).value<QSharedPointer<const User> >();
    QCOMPARE(u3->getName(), u->getName());

    QSignalSpy spy6(chat.data(), SIGNAL(sendCompleted(bool, const QString&)));
    QVERIFY(spy6.isValid());
    QVERIFY(spy6.isEmpty());

    QSignalSpy spy7(chat2.data(), SIGNAL(messageReceived(QSharedPointer<const User>, const QString&)));
    QVERIFY(spy7.isValid());
    QVERIFY(spy7.isEmpty());

    QSignalSpy spy8(chat.data(), SIGNAL(messageReceived(QSharedPointer<const User>, const QString&)));
    QVERIFY(spy8.isValid());
    QVERIFY(spy8.isEmpty());

    chat->send("Khaaan!!!");

    waitForResult(spy6);

    QList<QVariant> arguments5 = spy6.takeFirst();
    QVERIFY(arguments5.at(0) == true);

    waitForResult(spy7);

    QList<QVariant> arguments6 = spy7.takeFirst();
    QCOMPARE(arguments6.at(1).toString(), QString("Khaaan!!!"));
    QSharedPointer<const User> u4 = arguments6.at(0).value<QSharedPointer<const User> >();
    QCOMPARE(u4->getName(), QString("fefeb10c@selinux.inso.tuwien.ac.at"));

    waitForResult(spy8);

    QList<QVariant> arguments7 = spy8.takeFirst();
    QCOMPARE(arguments7.at(1).toString(), QString("Khaaan!!!"));
    QSharedPointer<const User> u5 = arguments7.at(0).value<QSharedPointer<const User> >();
    QCOMPARE(u5->getName(), QString("fefeb10c@selinux.inso.tuwien.ac.at"));
}

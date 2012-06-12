/* *************************************************************************
 *  Copyright 2012 TU VIENNA SEPM GROUP 22                                 *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */


#include "chattest.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include <QCryptographicHash>

#include "sessionmanager.h"
#include "user.h"
#include "loginuser.h"
#include "common.h"

QTEST_MAIN(ChatTests)

/* The preprocessor knows nothing about templates and complains
 * if the comma occurs in Q_DECLARE_METATYPE's parameter. */
#define QPAIR_HACK QList<QPair<QDateTime, QString> >

Q_DECLARE_METATYPE(QSharedPointer<Session>)
Q_DECLARE_METATYPE(QSharedPointer<Chat>)
Q_DECLARE_METATYPE(QSharedPointer<const User>)
Q_DECLARE_METATYPE(User)
Q_DECLARE_METATYPE(QPAIR_HACK)
Q_DECLARE_METATYPE(QList<ChatlogEntry>)

static void registerUser(const QString &user)
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager, SIGNAL(registerCompleted(bool, const QString &)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const User> u(new User(user, WORKING_DIR "public.pem"));
    sessionManager->registerUser(SERVER_URL, CA_CERT, u, "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);

    /* Don't check results - we don't delete this user, so it will fail. */
}

void ChatTests::initTestCase()
{
    qRegisterMetaType<QSharedPointer<Session> >("QSharedPointer<Session>");
    qRegisterMetaType<QSharedPointer<Chat> >("QSharedPointer<Chat>");
    qRegisterMetaType<QSharedPointer<const User> >("QSharedPointer<const User>");
    qRegisterMetaType<User>("User");
    qRegisterMetaType<QList<QPair<QDateTime, QString> > >(
        "QList<QPair<QDateTime, QString> >");
    qRegisterMetaType<QList<ChatlogEntry> >("QList<ChatlogEntry>");

#ifdef RUN_SERVER
    SPAWN_SERVER(server);
#endif

    registerUser(TEMP_SESSION_USER3);
    registerUser(TEMP_SESSION_USER2);
}

void ChatTests::cleanupTestCase()
{
#ifdef RUN_SERVER
    TERMINATE_SERVER(server);
#endif
}

void ChatTests::init()
{
    /* User #1. */

    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>, bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const LoginUser> u(new LoginUser(TEMP_SESSION_USER3,
                                      WORKING_DIR "public.pem", WORKING_DIR "private.pem"));
    sessionManager->login(SERVER_URL, CA_CERT, u, "password");

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

    /* User #2. */

    QSignalSpy spy3(sessionManager,
                    SIGNAL(loginCompleted(QSharedPointer<Session>, bool, QString)));
    QVERIFY(spy3.isValid());
    QVERIFY(spy3.isEmpty());

    QSharedPointer<const LoginUser> u2(new LoginUser(TEMP_SESSION_USER2,
                                       WORKING_DIR "public.pem", WORKING_DIR "private.pem"));
    sessionManager->login(SERVER_URL, CA_CERT, u2, "password");

    waitForResult(spy3);
    QCOMPARE(spy3.count(), 1);
    QList<QVariant> arguments3 = spy3.takeFirst();
    QVERIFY2(arguments3.at(1) == true, arguments3.at(2).toString().toStdString().c_str());

    session2 = arguments3.at(0).value<QSharedPointer<Session> >();
}

void ChatTests::cleanup()
{
    /* User #1. */

    QVERIFY(session);

    QSignalSpy spy(session.data(), SIGNAL(logoutCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->logout();
    waitForResult(spy);

    /* User #2. */

    QVERIFY(session2);

    QSignalSpy spy2(session2.data(), SIGNAL(logoutCompleted(bool, QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    session2->logout();
    waitForResult(spy2);
}

void ChatTests::testInviteNotLoggedIn()
{
    QVERIFY(chat);

    QSignalSpy spy(chat.data(), SIGNAL(inviteCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const User> u(new User(getUsername("registeredbutneverlogsin"),
                                          WORKING_DIR "public.pem"));
    chat->invite(u);
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == false, arguments.at(1).toString().toStdString().c_str());
}

void ChatTests::testInviteNonexistent()
{
    QVERIFY(chat);

    QSignalSpy spy(chat.data(), SIGNAL(inviteCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const User> u(new User(getUsername("thisuserbetternotexist"),
                                          WORKING_DIR "public.pem"));
    chat->invite(u);
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == false, arguments.at(1).toString().toStdString().c_str());
}

void ChatTests::testInvite()
{
    QVERIFY(chat);

    QSignalSpy spy2(chat.data(), SIGNAL(inviteCompleted(bool, QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    QSignalSpy spy3(session2.data(), SIGNAL(invitationReceived(QSharedPointer<Chat>)));
    QVERIFY(spy3.isValid());
    QVERIFY(spy3.isEmpty());

    QSignalSpy spy4(chat.data(), SIGNAL(userJoined(QSharedPointer<const User>)));
    QVERIFY(spy4.isValid());
    QVERIFY(spy4.isEmpty());

    QSharedPointer<const User> u(new User(TEMP_SESSION_USER2,
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
    QCOMPARE(u2->getName(), QString(TEMP_SESSION_USER2));

    QList<QSharedPointer<const User> > chat1Usr = chat->getUserList();
    QList<QSharedPointer<const User> > chat2Usr = chat2->getUserList();

    QCOMPARE(chat1Usr.size(), 2);
    QCOMPARE(chat2Usr.size(), 2);

    QCOMPARE(chat1Usr[0]->getName(), QString(TEMP_SESSION_USER2));
    QCOMPARE(chat1Usr[1]->getName(), QString(TEMP_SESSION_USER3));

    QCOMPARE(chat2Usr[0]->getName(), QString(TEMP_SESSION_USER2));
    QCOMPARE(chat2Usr[1]->getName(), QString(TEMP_SESSION_USER3));
}

void ChatTests::testEchoMessage()
{
    QVERIFY(chat);

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
    QVERIFY(chat);

    QSignalSpy spy(session2.data(), SIGNAL(invitationReceived(QSharedPointer<Chat>)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSignalSpy spy2(chat.data(), SIGNAL(userJoined(QSharedPointer<const User>)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    QSharedPointer<const User> u(new User(TEMP_SESSION_USER2,
                                          WORKING_DIR "public.pem"));
    chat->invite(u);
    waitForResult(spy);

    QList<QVariant> arguments = spy.takeFirst();
    QSharedPointer<Chat> chat2 = arguments.at(0).value<QSharedPointer<Chat> >();

    waitForResult(spy2);

    QSignalSpy spy3(chat2.data(), SIGNAL(sendCompleted(bool, const QString &)));
    QVERIFY(spy3.isValid());
    QVERIFY(spy3.isEmpty());

    QSignalSpy spy4(chat.data(), SIGNAL(messageReceived(QSharedPointer<const User>, const QString &)));
    QVERIFY(spy4.isValid());
    QVERIFY(spy4.isEmpty());

    QSignalSpy spy5(chat2.data(), SIGNAL(messageReceived(QSharedPointer<const User>, const QString &)));
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

    QSignalSpy spy6(chat.data(), SIGNAL(sendCompleted(bool, const QString &)));
    QVERIFY(spy6.isValid());
    QVERIFY(spy6.isEmpty());

    QSignalSpy spy7(chat2.data(), SIGNAL(messageReceived(QSharedPointer<const User>, const QString &)));
    QVERIFY(spy7.isValid());
    QVERIFY(spy7.isEmpty());

    QSignalSpy spy8(chat.data(), SIGNAL(messageReceived(QSharedPointer<const User>, const QString &)));
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
    QCOMPARE(u4->getName(), QString(TEMP_SESSION_USER3));

    waitForResult(spy8);

    QList<QVariant> arguments7 = spy8.takeFirst();
    QCOMPARE(arguments7.at(1).toString(), QString("Khaaan!!!"));
    QSharedPointer<const User> u5 = arguments7.at(0).value<QSharedPointer<const User> >();
    QCOMPARE(u5->getName(), QString(TEMP_SESSION_USER3));
}

void ChatTests::testLeaveChat()
{
    QVERIFY(chat);

    QSignalSpy spy(session2.data(), SIGNAL(invitationReceived(QSharedPointer<Chat>)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSignalSpy spy4(chat.data(), SIGNAL(userJoined(QSharedPointer<const User>)));
    QVERIFY(spy4.isValid());
    QVERIFY(spy4.isEmpty());

    QSharedPointer<const User> u(new User(TEMP_SESSION_USER2,
                                          WORKING_DIR "public.pem"));
    chat->invite(u);
    waitForResult(spy);
    waitForResult(spy4);

    QList<QVariant> arguments = spy.takeFirst();
    QSharedPointer<Chat> chat2 = arguments.at(0).value<QSharedPointer<Chat> >();

    QSignalSpy spy2(chat.data(), SIGNAL(leaveChatCompleted(bool, QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    QSignalSpy spy3(chat2.data(), SIGNAL(userLeft(QSharedPointer<const User>)));
    QVERIFY(spy3.isValid());
    QVERIFY(spy3.isEmpty());

    chat->leaveChat();

    waitForResult(spy2);
    waitForResult(spy3);

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> arguments2 = spy2.takeFirst();
    QVERIFY2(arguments2.at(0) == true, arguments2.at(1).toString().toStdString().c_str());

    QCOMPARE(spy3.count(), 1);
    QList<QVariant> arguments3 = spy3.takeFirst();
    QSharedPointer<const User> u2 = arguments3.at(0).value<QSharedPointer<const User> >();
    QCOMPARE(u2->getName(), QString(TEMP_SESSION_USER3));
}

void ChatTests::testRetrieveLog()
{
    testTransmitMessage();

    QSignalSpy spy3(chat.data(), SIGNAL(leaveChatCompleted(bool, QString)));
    QVERIFY(spy3.isValid());
    QVERIFY(spy3.isEmpty());

    chat->leaveChat();

    waitForResult(spy3);

    QSignalSpy spy(session.data(), SIGNAL(retrieveLoglistCompleted(const
                                          QList<QPair<QDateTime, QString> >, bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->retrieveLoglist();

    waitForResult(spy);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());
    QList<QPair<QDateTime, QString> > logs = arguments.at(0).value<
            QList<QPair<QDateTime, QString> > >();
    QVERIFY(logs.size() > 0);

    QSignalSpy spy2(session.data(), SIGNAL(retrieveLogCompleted(const
                                           QList<ChatlogEntry>, bool, QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    session->retrieveLog(logs[logs.size()-1]);

    waitForResult(spy2);

    QList<QVariant> arguments2 = spy2.takeFirst();
    QVERIFY2(arguments2.at(1) == true, arguments2.at(2).toString().toStdString().c_str());
    QList<ChatlogEntry> log = arguments2.at(0).value<QList<ChatlogEntry> >();
    QVERIFY(log.size() == 2);

    QCOMPARE(log[0].getMessage(), QString("Cupcakes!!!"));
    QCOMPARE(log[1].getMessage(), QString("Khaaan!!!"));
}

void ChatTests::testLeaveChatReinvite()
{
    QSignalSpy spy(session2.data(), SIGNAL(invitationReceived(QSharedPointer<Chat>)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSignalSpy spy4(chat.data(), SIGNAL(userJoined(QSharedPointer<const User>)));
    QVERIFY(spy4.isValid());
    QVERIFY(spy4.isEmpty());

    QSharedPointer<const User> u(new User(TEMP_SESSION_USER2,
                                          WORKING_DIR "public.pem"));
    chat->invite(u);
    waitForResult(spy);
    waitForResult(spy4);

    QList<QVariant> arguments = spy.takeFirst();
    QSharedPointer<Chat> chat2 = arguments.at(0).value<QSharedPointer<Chat> >();

    QSignalSpy spy2(chat2.data(), SIGNAL(leaveChatCompleted(bool, QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    QSignalSpy spy3(chat.data(), SIGNAL(userLeft(QSharedPointer<const User>)));
    QVERIFY(spy3.isValid());
    QVERIFY(spy3.isEmpty());

    chat2->leaveChat();

    waitForResult(spy2);
    waitForResult(spy3);

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> arguments2 = spy2.takeFirst();
    QVERIFY2(arguments2.at(0) == true, arguments2.at(1).toString().toStdString().c_str());

    QCOMPARE(spy3.count(), 1);
    QList<QVariant> arguments3 = spy3.takeFirst();
    QSharedPointer<const User> u2 = arguments3.at(0).value<QSharedPointer<const User> >();
    QCOMPARE(u2->getName(), QString(TEMP_SESSION_USER2));

    QSignalSpy spy9(session2.data(), SIGNAL(invitationReceived(QSharedPointer<Chat>)));
    QVERIFY(spy9.isValid());
    QVERIFY(spy9.isEmpty());

    chat->invite(u);
    waitForResult(spy9);
    QCOMPARE(spy9.count(), 1);
}

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


#include "sessiontest.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include <QCryptographicHash>

#include "sessionmanager.h"
#include "user.h"
#include "common.h"

QTEST_MAIN(SessionTests)

/* The preprocessor knows nothing about templates and complains
 * if the comma occurs in Q_DECLARE_METATYPE's parameter. */
#define QPAIR_HACK QList<QPair<QDateTime, QString> >

Q_DECLARE_METATYPE(QSharedPointer<Session>)
Q_DECLARE_METATYPE(QSharedPointer<const User>)
Q_DECLARE_METATYPE(QPAIR_HACK)
Q_DECLARE_METATYPE(QList<ChatlogEntry>)
void SessionTests::initTestCase()
{
    qRegisterMetaType<QSharedPointer<Session> >("QSharedPointer<Session>");
    qRegisterMetaType<QSharedPointer<Chat> >("QSharedPointer<Chat>");
    qRegisterMetaType<QSharedPointer<const User> >("QSharedPointer<const User>");
    qRegisterMetaType<const QObject *>("const QObject*");
    qRegisterMetaType<QList<QPair<QDateTime, QString> > >(
        "QList<QPair<QDateTime, QString> >");
    qRegisterMetaType<QList<ChatlogEntry> >("QList<ChatlogEntry>");

#ifdef RUN_SERVER
    SPAWN_SERVER(server);
#endif
}

void SessionTests::cleanupTestCase()
{
#ifdef RUN_SERVER
    TERMINATE_SERVER(server);
#endif
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
    sessionManager->login(SERVER_URL, CA_CERT, u,
                          "password");

    waitForResult(spy);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());

    session = arguments.at(0).value<QSharedPointer<Session> >();
    QVERIFY(session);
}

void SessionTests::cleanup()
{
    QVERIFY(session);

    if (!session->isValid())
        return;

    QSignalSpy spy(session.data(), SIGNAL(logoutCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->logout();
    waitForResult(spy);
}

void SessionTests::testInitChat()
{
    QVERIFY(session);

    QSignalSpy spy(session.data(), SIGNAL(initChatCompleted(QSharedPointer<Chat>, bool,
                                          QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->initChat();
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(1) == true, arguments.at(2).toString().toStdString().c_str());

    QVERIFY(session->isValid());
}

void SessionTests::testLogout()
{
    QVERIFY(session);

    QSignalSpy spy(session.data(), SIGNAL(logoutCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->logout();
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == true, arguments.at(1).toString().toStdString().c_str());

    QVERIFY(!session->isValid());
}

void SessionTests::testDeleteUserNonexistent()
{
    QVERIFY(session);

    QSignalSpy spy(session.data(), SIGNAL(deleteUserCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->deleteUser(QSharedPointer<const User>(new User(
                            getUsername("thisuserbetternotexist"),
                            WORKING_DIR "public.pem")));
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == false, arguments.at(1).toString().toStdString().c_str());
}

void SessionTests::testDeleteUser()
{
    QVERIFY(session);

    QSignalSpy spy(session.data(), SIGNAL(deleteUserCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->deleteUser(QSharedPointer<const User>(new User(TEMP_SESSION_USER,
                        WORKING_DIR "public.pem")));
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == true, arguments.at(1).toString().toStdString().c_str());

    QVERIFY(!session->isValid());
}

void SessionTests::testDeleteUserUnauthorized()
{
    QVERIFY(session);

    QSignalSpy spy(session.data(), SIGNAL(deleteUserCompleted(bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->deleteUser(QSharedPointer<const User>(new User(getUsername("fefeb10c"),
                        WORKING_DIR "public.pem")));
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0) == false);
}

void SessionTests::retrieveUser()
{
    QVERIFY(session);

    QSignalSpy spy(session.data(), SIGNAL(retrieveUserCompleted(QSharedPointer<const User>,
                                          const QObject *, bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    const QString username = TEMP_SESSION_USER;

    session->retrieveUser(username, this);
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(2) == true, arguments.at(3).toString().toStdString().c_str());

    QSharedPointer<const User> usr = arguments.at(0).value<QSharedPointer<const User> >();
    QCOMPARE(usr.data()->getName(), username);
}

void SessionTests::retrieveUserNonexistent()
{
    QVERIFY(session);

    QSignalSpy spy(session.data(), SIGNAL(retrieveUserCompleted(QSharedPointer<const User>,
                                          const QObject *, bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    session->retrieveUser(getUsername("thisuserbetternotexist"), this);
    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(2) == false, arguments.at(3).toString().toStdString().c_str());
}

void SessionTests::retrieveEmptyLoglist()
{
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
    QVERIFY(logs.size() == 0);
}

void SessionTests::retrieveNonEmptyLoglist()
{
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
    QVERIFY(logs.size() == 1);

    QSignalSpy spy2(session.data(), SIGNAL(retrieveLogCompleted(const
                                           QList<ChatlogEntry>, bool, QString)));
    QVERIFY(spy2.isValid());
    QVERIFY(spy2.isEmpty());

    session->retrieveLog(logs[0]);

    waitForResult(spy2);

    QList<QVariant> arguments2 = spy2.takeFirst();
    QVERIFY2(arguments2.at(1) == true, arguments2.at(2).toString().toStdString().c_str());
    QList<ChatlogEntry> log = arguments2.at(0).value<QList<ChatlogEntry> >();
    QVERIFY(log.size() == 0);
}

void SessionTests::retrieveContactListNonexistent()
{
    QVERIFY(session);

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
    QVERIFY(contactsRet.size() == 0);
}

void SessionTests::saveRetrieveContactList()
{
    QVERIFY(session);

    QStringList contacts = QString("%1\n%2").arg(getUsername("pinkie_pie"),
                           getUsername("rainbow_dash")).split('\n');

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

void SessionTests::saveRetrieveEmptyContactList()
{
    QVERIFY(session);

    QStringList contacts;

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
    QVERIFY(contactsRet.size() == 0);
}

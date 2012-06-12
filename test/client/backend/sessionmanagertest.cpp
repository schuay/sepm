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


#include "sessionmanagertest.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include <QCryptographicHash>

#include "sessionmanager.h"
#include "user.h"
#include "loginuser.h"
#include "Security.h"
#include "common.h"

using namespace sdcc;

QTEST_MAIN(SessionManagerTests)

Q_DECLARE_METATYPE(QSharedPointer<Session>)
void SessionManagerTests::initTestCase()
{
    qRegisterMetaType<QSharedPointer<Session> >("QSharedPointer<Session>");

#ifdef RUN_SERVER
    SPAWN_SERVER(server);
#endif
}

void SessionManagerTests::cleanupTestCase()
{
#ifdef RUN_SERVER
    TERMINATE_SERVER(server);
#endif
}

void SessionManagerTests::testTestConnection()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(testConnectionCompleted(bool, const QString &)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    sessionManager->testConnection(SERVER_URL);

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

void SessionManagerTests::testLoginUser()
{
    try {
        sdc::Security s;
        sdc::ByteSeq testKey = s.genAESKey(128);
        sdc::ByteSeq cipherText = s.encryptRSA(WORKING_DIR "public.pem", testKey);
        LoginUser u("user@example.com", WORKING_DIR "public.pem",
                    WORKING_DIR "private.pem");
        sdc::ByteSeq plainText = u.decrypt(cipherText);
        sdc::ByteSeq signature = u.sign(testKey);
        QCOMPARE(plainText, testKey);
        QCOMPARE(s.verifyRSA(WORKING_DIR "public.pem", testKey, signature), true);
    } catch (sdc::SecurityException s) {
        QFAIL(s.what());
    }
}

void SessionManagerTests::testRegisterUserNew()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(registerCompleted(bool, const QString &)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const User> u(new User(TEMP_SESSION_USER, WORKING_DIR "public.pem"));
    sessionManager->registerUser(SERVER_URL, CA_CERT, u,
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
    QSharedPointer<const User> u(new User(getUsername(randomName.left(10)),
                                          WORKING_DIR "public.pem"));
    sessionManager->registerUser(SERVER_URL, CA_CERT, u, "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == true, arguments.at(1).toString().toStdString().c_str());
}

void SessionManagerTests::testRegisterUserIncorrectDomain()
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
    QSharedPointer<const User> u(new User(QString("%1@%2").arg(randomName.left(10), "made.up.domain"),
                                          WORKING_DIR "public.pem"));
    sessionManager->registerUser(SERVER_URL, CA_CERT, u, "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY2(arguments.at(0) == false, arguments.at(1).toString().toStdString().c_str());
}

void SessionManagerTests::testRegisterUserAgain()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(registerCompleted(bool, const QString &)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const User> u(new User(TEMP_SESSION_USER, WORKING_DIR "public.pem"));
    sessionManager->registerUser(SERVER_URL, CA_CERT, u, "password");

    waitForResult(spy);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0) == false);
}

void SessionManagerTests::testLoginNonexistentUser()
{
    SessionManager *sessionManager = SessionManager::getInstance();
    QSignalSpy spy(sessionManager,
                   SIGNAL(loginCompleted(QSharedPointer<Session>, bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const LoginUser> u(new LoginUser(getUsername("thisuserbetternotexist"),
                                      WORKING_DIR "public.pem", WORKING_DIR "private.pem"));
    sessionManager->login(SERVER_URL, CA_CERT, u,
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
                   SIGNAL(loginCompleted(QSharedPointer<Session>, bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const LoginUser> u(new LoginUser(getUsername("fefeb10c"),
                                      WORKING_DIR "public.pem", WORKING_DIR "private.pem"));
    sessionManager->login(SERVER_URL, CA_CERT, u,
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
                   SIGNAL(loginCompleted(QSharedPointer<Session>, bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const LoginUser> u(new LoginUser(TEMP_SESSION_USER,
                                      WORKING_DIR "public.pem", WORKING_DIR "private.pem"));
    sessionManager->login(SERVER_URL, CA_CERT, u,
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
                   SIGNAL(loginCompleted(QSharedPointer<Session>, bool, QString)));
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    QSharedPointer<const LoginUser> u(new LoginUser(TEMP_SESSION_USER,
                                      WORKING_DIR "public.pem", WORKING_DIR "private.pem"));
    sessionManager->login(SERVER_URL, CA_CERT, u, "password");

    waitForResult(spy, 1);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
#ifndef RUN_SERVER
    QEXPECT_FAIL("", "The reference server does not implement multiple logins correctly.", Abort);
#endif
    QVERIFY2(arguments.at(1) == false, arguments.at(2).toString().toStdString().c_str());
}

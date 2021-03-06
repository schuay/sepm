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


#include "userdbproxytest.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include "userdbproxy.h"
#include "common.h"
#include "sdcHelper.h"
#include "settings.h"

using namespace sdcs;

QTEST_MAIN(UserDbProxyTests)

Q_DECLARE_METATYPE(QSharedPointer<UserDbProxy>)
void UserDbProxyTests::initTestCase()
{
    qRegisterMetaType<QSharedPointer<UserDbProxy> >("QSharedPointer<UserDbProxy>");

    sdc::Settings::init(BINARY_DIR "/test/sdcs.conf");

    const unsigned char pubkey[] = { 'b', 'l', 'a' };
    user1.ID = "test1";
    user1.publicKey = sdc::ByteSeq(pubkey, pubkey + sizeof(pubkey));
    hash1 = QByteArray("123");
    salt1 = QByteArray("456");

    db = QSqlDatabase::addDatabase(sdc::Settings::getValue(sdc::Settings::SDbDriver).toString());
    db.setHostName(sdc::Settings::getValue(sdc::Settings::SDbHost).toString());
    db.setDatabaseName(sdc::Settings::getValue(sdc::Settings::SDbDatabase).toString());
    db.setUserName(sdc::Settings::getValue(sdc::Settings::SDbUser).toString());
    db.setPassword(sdc::Settings::getValue(sdc::Settings::SDbPassword).toString());

    QVERIFY(db.open());

    QSqlQuery query;
    QVERIFY(query.exec("truncate table public.user cascade;"));
    QVERIFY(query.exec("select setval('user_id_seq', 1);"));
    QVERIFY(query.exec("insert into public.user(username, public_key, password_hash, salt) select 'test1', 'bla', '123', '456';"));
    QVERIFY(query.exec("insert into public.user(username, public_key, password_hash, salt) select 'test2', 'bla', 'bla', 'bla';"));
    QVERIFY(query.exec("insert into public.user(username, public_key, password_hash, salt) select 'test3', 'bla', 'bla', 'bla';"));
    QVERIFY(query.exec("insert into public.contactlist(user_id, encrypted_content, signature)"
                       "values (2, 'abfg', 'faslkd'),"
                       "       (3, 'jhas', 'sfalhf');"));
    QVERIFY(query.exec("insert into public.chatlog(user_id, time_stamp, chat_id, encrypted_content, signature)"
                       "values (2, 0, 'chat0', 'abaskdj', 'salfkhaf'),"
                       "       (2, 1, 'chat0', 'sajdhas', 'a;lghqes'),"
                       "       (3, 0, 'chat0', 'aslfkaf', 'asflkajf');"));
}


void UserDbProxyTests::cleanupTestCase()
{
    db.close();
}

void UserDbProxyTests::testDeleteExistentUser()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy("test2");
    p->deleteUser();

    QSqlQuery query(db);
    query.exec("select * from public.user where username = 'test2';");

    QCOMPARE(query.size(), 0);
}

void UserDbProxyTests::testDeleteNonexistentUser()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy("test3");
    p->deleteUser();

    QSqlQuery query(db);
    query.exec("select * from public.user where username = 'test3';");

    QCOMPARE(query.size(), 0);

    /* This should not throw an exception (even though the user doesn't exist) */
    p->deleteUser();
}

void UserDbProxyTests::testCreateUser()
{
    const unsigned char pub[] = "abcd";
    const char hash[] = "abcd";

    sdc::User u;
    u.ID = "test99";
    u.publicKey = sdc::ByteSeq(pub, pub + sizeof(pub));

    UserDbProxy::createUser(u, QByteArray(hash, sizeof(hash)),
                            QByteArray(hash, sizeof(hash)));

    QSqlQuery query(db);
    query.exec("select * from public.user where username = 'test99';");

    QCOMPARE(query.size(), 1);
}

void UserDbProxyTests::testCreateExistingUser()
{
    const unsigned char pub[] = "abcd";
    const char hash[] = "abcd";

    sdc::User u;
    u.ID = "test99";
    u.publicKey = sdc::ByteSeq(pub, pub + sizeof(pub));

    QVERIFY_THROW(UserDbProxy::createUser(u, QByteArray(hash, sizeof(hash)),
                                          QByteArray(hash, sizeof(hash))),
                  sdc::UserHandlingException);
}

void UserDbProxyTests::testConnection()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy(QString::fromStdString(user1.ID));
}

void UserDbProxyTests::testRetrieveUser()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy(QString::fromStdString(user1.ID));

    sdc::User u = p->getUser();
    QCOMPARE(u.ID, user1.ID);
    QCOMPARE(u.publicKey, user1.publicKey);
    QCOMPARE(p->getHash(), hash1);
    QCOMPARE(p->getSalt(), salt1);
}

void UserDbProxyTests::testRetrieveNonexistentUser()
{
    QVERIFY_THROW(UserDbProxy::getProxy("thisuserdoesntexist"), sdc::UserHandlingException);
}

void UserDbProxyTests::testRetrieveContactList()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy(QString::fromStdString(user1.ID));

    sdc::SecureContainer container = p->retrieveContactList();

    QCOMPARE(QByteArray("abfg"), sdc::sdcHelper::byteSeqToByteArray(container.data));
    QCOMPARE(QByteArray("faslkd"), sdc::sdcHelper::byteSeqToByteArray(container.signature));
}

void UserDbProxyTests::testRetrieveNonexistentContactList()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy("test3");

    sdc::SecureContainer container = p->retrieveContactList();
    sdc::SecureContainer empty;

    QCOMPARE(empty.data, container.data);
    QCOMPARE(empty.signature, container.signature);

}

void UserDbProxyTests::testSaveContactList()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy(QString::fromStdString(user1.ID));

    sdc::SecureContainer in;
    in.data = sdc::sdcHelper::byteArraytoByteSeq(QByteArray("918273"));
    in.signature = sdc::sdcHelper::byteArraytoByteSeq(QByteArray("837192"));

    p->saveContactList(in);

    sdc::SecureContainer out = p->retrieveContactList();
    QCOMPARE(in.data, out.data);
    QCOMPARE(in.signature, out.signature);
}

void UserDbProxyTests::testSaveContactListAgain()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy(QString::fromStdString(user1.ID));

    sdc::SecureContainer in;
    in.data = sdc::sdcHelper::byteArraytoByteSeq(QByteArray("124124"));
    in.signature = sdc::sdcHelper::byteArraytoByteSeq(QByteArray("124124532"));

    p->saveContactList(in);

    sdc::SecureContainer out = p->retrieveContactList();
    QCOMPARE(in.data, out.data);
    QCOMPARE(in.signature, out.signature);
}

void UserDbProxyTests::testRetrieveLoglist()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy(QString::fromStdString(user1.ID));

    sdc::Loglist list = p->retrieveLoglist();

    QCOMPARE(static_cast<sdc::Loglist::size_type>(2), list.size());

    QCOMPARE("chat0", list[0].chatID.c_str());
    QCOMPARE(static_cast<Ice::Long>(0), list[0].timestamp);

    QCOMPARE("chat0", list[1].chatID.c_str());
    QCOMPARE(static_cast<Ice::Long>(1), list[1].timestamp);
}

void UserDbProxyTests::testRetrieveLoglistEmpty()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy("test3");

    sdc::Loglist list = p->retrieveLoglist();

    QCOMPARE(static_cast<sdc::Loglist::size_type>(0), list.size());
}

void UserDbProxyTests::testRetrieveLog()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy(QString::fromStdString(user1.ID));

    sdc::SecureContainer out = p->retrieveLog("chat0", 0);

    QCOMPARE(QByteArray("abaskdj"), sdc::sdcHelper::byteSeqToByteArray(out.data));
    QCOMPARE(QByteArray("salfkhaf"), sdc::sdcHelper::byteSeqToByteArray(out.signature));
}

void UserDbProxyTests::testRetrieveLogNonexistent()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy(QString::fromStdString(user1.ID));

    QVERIFY_THROW(sdc::SecureContainer out = p->retrieveLog("chat0", 1234), sdc::LogException);
}

void UserDbProxyTests::testSaveLog()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy(QString::fromStdString(user1.ID));

    sdc::SecureContainer in;
    in.data = sdc::sdcHelper::byteArraytoByteSeq(QByteArray("918273"));
    in.signature = sdc::sdcHelper::byteArraytoByteSeq(QByteArray("837192"));

    p->saveLog("chat2", 1209124, in);

    sdc::SecureContainer out = p->retrieveLog("chat2", 1209124);

    QCOMPARE(in.data, out.data);
    QCOMPARE(in.signature, out.signature);
}

void UserDbProxyTests::testSaveLogDuplicate()
{
    QSharedPointer<UserDbProxy> p = UserDbProxy::getProxy(QString::fromStdString(user1.ID));

    sdc::SecureContainer in;
    in.data = sdc::sdcHelper::byteArraytoByteSeq(QByteArray("1234567"));
    in.signature = sdc::sdcHelper::byteArraytoByteSeq(QByteArray("276123"));

    p->saveLog("chat2", 1209124, in);

    sdc::SecureContainer out = p->retrieveLog("chat2", 1209124);

    QCOMPARE(in.data, out.data);
    QCOMPARE(in.signature, out.signature);
}

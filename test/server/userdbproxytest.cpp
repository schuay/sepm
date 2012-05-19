#include "userdbproxytest.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include "userdbproxy.h"
#include "common.h"

using namespace sdcs;

QTEST_MAIN(UserDbProxyTests)

Q_DECLARE_METATYPE(QSharedPointer<UserDbProxy>)
void UserDbProxyTests::initTestCase()
{
    qRegisterMetaType<QSharedPointer<UserDbProxy> >("QSharedPointer<UserDbProxy>");

    UserDbProxy::setDriver(DB_DRIVER);
    UserDbProxy::setHost(DB_HOST);
    UserDbProxy::setDatabase(DB_DATABASE);
    UserDbProxy::setUser(DB_USER);
    UserDbProxy::setPassword(DB_PASSWORD);

    const unsigned char pubkey[] = { 'b', 'l', 'a' };
    user1.ID = "test1";
    user1.publicKey = sdc::ByteSeq(pubkey, pubkey + sizeof(pubkey));
    hash1 = QByteArray("123");
    salt1 = QByteArray("456");

    db = QSqlDatabase::addDatabase(DB_DRIVER);
    db.setHostName(DB_HOST);
    db.setDatabaseName(DB_DATABASE);
    db.setUserName(DB_USER);
    db.setPassword(DB_PASSWORD);

    QVERIFY(db.open());

    QSqlQuery query;
    QVERIFY(query.exec("truncate table public.user cascade;"));
    QVERIFY(query.exec("insert into public.user(username, public_key, password_hash, salt) select 'test1', 'bla', '123', '456';"));
    QVERIFY(query.exec("insert into public.user(username, public_key, password_hash, salt) select 'test2', 'bla', 'bla', 'bla';"));
    QVERIFY(query.exec("insert into public.user(username, public_key, password_hash, salt) select 'test3', 'bla', 'bla', 'bla';"));
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

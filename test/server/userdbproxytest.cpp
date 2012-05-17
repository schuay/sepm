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
    const unsigned char hash[] = { 'b', 'l', 'a' };
    user1.ID = "test1";
    user1.publicKey = sdc::ByteSeq(pubkey, pubkey + sizeof(pubkey));
    hash1 = sdc::ByteSeq(hash, hash + sizeof(hash));
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
}

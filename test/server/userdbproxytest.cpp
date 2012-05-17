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
}

void UserDbProxyTests::testConnection()
{
}

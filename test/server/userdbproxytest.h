#include <QObject>
#include <QSqlQuery>

#include "SecureDistributedChat.h"

class UserDbProxyTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testConnection();
    void testRetrieveUser();
    void testDeleteExistentUser();
    void testDeleteNonexistentUser();
    void cleanupTestCase();

private:
    sdc::User user1;
    sdc::ByteSeq hash1;

    QSqlDatabase db;
};

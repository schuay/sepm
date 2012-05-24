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
    void testRetrieveNonexistentUser();

    void testRetrieveContactList();
    void testRetrieveNonexistentContactList();
    void testSaveContactList();
    void testSaveContactListAgain();

    void testRetrieveLoglist();
    void testRetrieveLoglistEmpty();
    void testRetrieveLog();
    void testRetrieveLogNonexistent();
    void testSaveLog();
    void testSaveLogDuplicate();

    void testDeleteExistentUser();
    void testDeleteNonexistentUser();

    void testCreateUser();
    void testCreateExistingUser();

    void cleanupTestCase();

private:
    sdc::User user1;
    QByteArray hash1;
    QByteArray salt1;

    QSqlDatabase db;
};

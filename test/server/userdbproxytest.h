#include <QObject>

#include "SecureDistributedChat.h"

class UserDbProxyTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testConnection();
    void testRetrieveUser();

private:
    sdc::User user1;
    sdc::ByteSeq hash1;
};

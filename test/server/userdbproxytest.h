#include <QObject>

class UserDbProxyTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testConnection();
};

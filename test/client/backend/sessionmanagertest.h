#include <QObject>
#include <QProcess>

class SessionManagerTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testTestConnection();
    void testUserCtorNamePath();
    void testUserCtorSdcUser();
    void testUserGetIceUser();
    void testLoginUser();
    void testRegisterUserNew();
    void testRegisterUserRandom();
    void testRegisterUserAgain();
    void testLoginNonexistentUser();
    void testLoginIncorrectCredentials();
    void testLoginCorrectCredentials();
    void testLoginRepeated();

private:
    QProcess server;
};

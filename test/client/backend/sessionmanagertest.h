#include <QObject>

class SessionManagerTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testTestConnection();
    void testUserCtorNamePath();
    void testUserCtorSdcUser();
    void testUserGetIceUser();
    void testRegisterUserNew();
    void testRegisterUserRandom();
    void testRegisterUserAgain();
    void testLoginNonexistentUser();
    void testLoginIncorrectCredentials();
    void testLoginCorrectCredentials();
    void testLoginRepeated();
};

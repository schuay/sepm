#include <QObject>

/**
 * Just a little tester for anything in the backend. Feel free to modify and
 * destroy it however you like to...
 **/

class ClientBackendTests : public QObject
{
    Q_OBJECT

private slots:
    void testTestConnection();
    void testUserCtorNamePath();
    void testUserCtorSdcUser();
    void testUserGetIceUser();
    void testRegisterUserNew();
    void testRegisterUserAgain();
};

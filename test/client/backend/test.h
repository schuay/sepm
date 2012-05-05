#include <QObject>

/**
 * Just a little tester for anything in the backend. Feel free to modify and
 * destroy it however you like to...
 **/

class ClientBackendTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testTestConnection();
    void testUserCtorNamePath();
    void testUserCtorSdcUser();
    void testUserGetIceUser();
    void testRegisterUserNew();
    void testRegisterUserAgain();
    void testLoginNonexistentUser();
    void testLoginIncorrectCredentials();
    void testLoginCorrectCredentials();
    void testLoginRepeated();
    void testLogout();

    /* This test is commented intentionally to show that this
     * behavior should actually be implemented. Currently, a repeated
     * logout just leaves the caller hanging without a reply.
     *
     * void testLogoutRepeated();
     */
};

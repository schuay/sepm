#include <QObject>
#include <QSharedPointer>
#include <QProcess>

#include "session.h"

using namespace sdcc;

class SessionTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();
    void retrieveEmptyLoglist();
    void testInitChat();
    void retrieveNonEmptyLoglist();
    void testLogout();
    void retrieveUser();
    void retrieveUserNonexistent();
    void retrieveContactListNonexistent();
    void saveRetrieveContactList();
    void saveRetrieveEmptyContactList();
    void testDeleteUserNonexistent();
    void testDeleteUserUnauthorized();
    void testDeleteUser(); /* This test must be last since it deletes our user. */

    /* This test is commented intentionally to show that this
     * behavior should actually be implemented. Currently, a repeated
     * logout just leaves the caller hanging without a reply.
     *
     * void testLogoutRepeated();
     */

private:
    QSharedPointer<Session> session;
    QProcess server;
};

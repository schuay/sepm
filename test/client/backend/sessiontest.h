#include <QObject>
#include <QSharedPointer>

#include "session.h"

using namespace sdcc;

class SessionTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();
    void testLogout();
    void testDeleteUserNonexistent();
    void testDeleteUser(); /* This test must be last since it deletes our user. */

    /* This test is commented intentionally to show that this
     * behavior should actually be implemented. Currently, a repeated
     * logout just leaves the caller hanging without a reply.
     *
     * void testLogoutRepeated();
     */

private:
    QSharedPointer<Session> session;
};

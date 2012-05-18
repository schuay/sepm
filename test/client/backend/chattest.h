#include <QObject>
#include <QSharedPointer>

#include "session.h"

using namespace sdcc;

class ChatTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();
    void testInviteNotLoggedIn();
    void testInviteNonexistent();
    void testInvite();
    void testEchoMessage();
    void testTransmitMessage();
    void testLeaveChat();

    /* This test is commented intentionally to show that this
     * behavior should actually be implemented. Currently, a repeated
     * logout just leaves the caller hanging without a reply.
     *
     * void testLogoutRepeated();
     */

private:
    QSharedPointer<Session> session;
    QSharedPointer<Session> session2;
    QSharedPointer<Chat> chat;
};

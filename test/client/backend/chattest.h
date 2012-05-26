#include <QObject>
#include <QSharedPointer>
#include <QProcess>

#include "session.h"

using namespace sdcc;

class ChatTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

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

    void testRetrieveLog();

private:
    QSharedPointer<Session> session;
    QSharedPointer<Session> session2;
    QSharedPointer<Chat> chat;
    QProcess server;
};

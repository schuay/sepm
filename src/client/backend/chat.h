#ifndef _CHAT_H
#define _CHAT_H

#include <QSharedPointer>

#include "user.h"

namespace sdcc
{

class Session;

/**
 * The local representation of a chat on a server (not necessarily
 * the one we are connected with).
 * At least one user will be involved in each chat, namely, the current
 * one. If we leave the chat, the object is destroyed.
 * Each chat belongs to a session and is destroyed if the session is closed.
 * All communication between the server and client is encrypted.
 * All method calls are asynchronous.
 */
class Chat : public QObject
{
    Q_OBJECT

public:

    /**
     * Leave the chat. After leaveChatCompleted(), the object is invalid
     * and must not be used anymore.
     * Returns immediately.
     */
    void leaveChat();

    /**
     * Invite another user to the chat. To get a User object, call
     * Session.retrieveUser(). Normally, both the signals inviteCompleted()
     * and userJoined() will be received.
     * Returns immediately.
     */
    void invite(const User &user);

    /**
     * Send a message to the chat.
     * Returns immediately.
     */
    void send(const QString &msg);

signals:

    /**
     * Another user sent a message to the chat.
     */
    void messageReceived(const User &user, const QString &msg);

    /**
     * Another user joined the chat.
     */
    void userJoined(const User &user);

    /**
     * Response signal for leaveChat.
     */
    void leaveChatCompleted(bool success, const QString &msg);

    /**
     * Response signal for invite.
     */
    void inviteCompleted(bool success, const QString &msg);

    /**
     * Response signal for send.
     */
    void sendCompleted(bool success, const QString &msg);

private:
    /* Prevent unintended construction of instances by user. */
    Chat(const Session &session, const QString &chatID);
    Chat(const Chat &);

    /**
     * All users in the chat.
     */
    QList<QSharedPointer<User> > users;

    /**
     * The ID of the chat.
     */
    const QString chatID;

    /**
     * The session this chat belongs to.
     */
    Session &session;
};

}

#endif /* _CHAT_H */

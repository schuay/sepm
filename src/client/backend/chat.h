/* *************************************************************************
 *  Copyright 2012 TU VIENNA SEPM GROUP 22                                 *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */


#ifndef _CHAT_H
#define _CHAT_H

#include <QSharedPointer>
#include <QMutexLocker>

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

    /* Allow SessionPrivate to call the private constructor. */
    friend class SessionPrivate;

public:

    /**
     * Returns the ID of the chat object.
     */
    const QString &getID() const;

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
    void invite(QSharedPointer<const User> user);

    /**
     * Send a message to the chat.
     * Returns immediately.
     */
    void send(const QString &msg);

    /**
     * Retrieves a list of the users currently in this chat.
     */
    QList<QSharedPointer<const User> > getUserList();

signals:

    /**
     * Another user sent a message to the chat.
     */
    void messageReceived(QSharedPointer<const User> user, const QString &msg);

    /**
     * Another user joined the chat.
     */
    void userJoined(QSharedPointer<const User> user);

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

    /**
     * A user left the chat.
     */
    void userLeft(QSharedPointer<const User> user);

    /**
     * Internal signal to notify the session that it can delete this chat.
     */
    void leaveChatCompleted(const QString &id);

private:
    /* Prevent unintended construction of instances by user. */
    Chat(sdc::SessionIPrx sessionPrx, Session &session,
         const QString &chatID, const sdc::ByteSeq key);
    Chat(const Chat &);

    /**
     * All users in the chat.
     */
    QMutex usersMutex;
    QMap<QString, QSharedPointer<const User> > users;

    /**
     * The ID of the chat.
     */
    const QString chatID;

    /**
     * The session this chat belongs to.
     */
    Session &session;

    /**
     * This chat's session proxy.
     */
    sdc::SessionIPrx sessionPrx;

    /**
     * The chat's session key.
     */
    sdc::ByteSeq key;

    /**
     * The chat's message log.
     */
    QMutex logMutex;
    sdc::Chatlog log;

    void runInvite(QSharedPointer<const User> user);
    void runLeaveChat();
    void runSend(const QString &msg);
    void receiveMessage(QSharedPointer<const User> participant, const sdc::ByteSeq &encMsg);
    void addChatParticipant(QSharedPointer<const User> participant);
    void removeChatParticipant(QSharedPointer<const User> participant);
};

}

#endif /* _CHAT_H */

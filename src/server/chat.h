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


#ifndef CHAT_H
#define CHAT_H

#include <QMap>
#include <QMutex>
#include <QSharedPointer>

#include "SecureDistributedChat.h"
#include "participant.h"

namespace sdcs
{

class Chat
{
public:
    Chat(const QString &chatID);
    virtual ~Chat();

    /**
     * Appends message from user to all participants.
     */
    virtual void appendMessageFrom(const sdc::User &user, const sdc::ByteSeq &message) = 0;

    /**
     * Invites user to the chat with the given session key.
     */
    virtual void inviteUser(const sdc::User &user, const sdc::ByteSeq &sessionKey) = 0;

    /**
     * Removes user from the chat and notifies all remaining participants.
     */
    virtual void leaveChat(const sdc::User &user) = 0;

    QString getChatID() const;

protected:

    const QString chatID;
};

/* owned by server, not by session. pointer kept in global list. destroyed only when empty. */
class LocalChat : public Chat
{
public:
    LocalChat(const QString &chatID, const sdc::User &user, sdc::ChatClientCallbackIPrx callback);

    void appendMessageFrom(const sdc::User &user, const sdc::ByteSeq &message);
    void inviteUser(const sdc::User &user, const sdc::ByteSeq &sessionKey);
    void leaveChat(const sdc::User &user);

private:

    QMap<QString, QSharedPointer<Participant> > participants;
    QMutex participantsMutex;
};

/* owned by session, pointer kept in session. destroyed on leave or logout. */
class RemoteChat : public Chat
{
public:
    RemoteChat(const QString &chatID);

    void appendMessageFrom(const sdc::User &user, const sdc::ByteSeq &message);
    void inviteUser(const sdc::User &user, const sdc::ByteSeq &sessionKey);
    void leaveChat(const sdc::User &user);

private:
    sdc::InterServerIPrx interServer; /* one per remote server */
};

}

#endif

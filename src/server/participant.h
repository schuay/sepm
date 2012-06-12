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


#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <QStringList>

#include "SecureDistributedChat.h"

namespace sdcs
{

/**
 * Represents a user participating in a chat.
 */
class Participant
{
public:
    Participant(const sdc::User &user, const QString &chatID);
    virtual ~Participant();

    QString getUserID();
    virtual void invite(const QStringList &users, const sdc::ByteSeq &sessionKey) = 0;
    virtual void addChatParticipant(const sdc::User &participant) = 0;
    virtual void removeChatParticipant(const sdc::User &participant) = 0;
    virtual void appendMessageToChat(const sdc::User &user, const sdc::ByteSeq &message) = 0;

protected:
    const sdc::User self;
    const QString chatID;
};

class LocalParticipant : public Participant
{
public:
    LocalParticipant(const sdc::User &user, sdc::ChatClientCallbackIPrx callback, const QString &chatID);
    LocalParticipant(const sdc::User &user, const QString &chatID);

    void invite(const QStringList &users, const sdc::ByteSeq &sessionKey);
    void addChatParticipant(const sdc::User &participant);
    void removeChatParticipant(const sdc::User &participant);
    void appendMessageToChat(const sdc::User &user, const sdc::ByteSeq &message);

private:
    void echo(const QString &message);

    sdc::ChatClientCallbackIPrx proxy;
};

class RemoteParticipant : public Participant
{
public:
    RemoteParticipant(const sdc::User &user, const QString &chatID);

    void invite(const QStringList &users, const sdc::ByteSeq &sessionKey);
    void addChatParticipant(const sdc::User &participant);
    void removeChatParticipant(const sdc::User &participant);
    void appendMessageToChat(const sdc::User &user, const sdc::ByteSeq &message);

private:
    sdc::InterServerIPrx proxy;
};

}

#endif

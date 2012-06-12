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


#include "participant.h"

#include "QsLog.h"
#include "server.h"
#include "sdcHelper.h"

namespace sdcs
{

Participant::Participant(const sdc::User &user, const QString &chatID)
    : self(user), chatID(chatID)
{
}

Participant::~Participant()
{
}

QString Participant::getUserID()
{
    return QString::fromStdString(self.ID);
}

LocalParticipant::LocalParticipant(const sdc::User &user, sdc::ChatClientCallbackIPrx callback,
                                   const QString &chatID)
    : Participant(user, chatID)
{
    proxy = callback;
}

LocalParticipant::LocalParticipant(const sdc::User &user, const QString &chatID)
    : Participant(user, chatID)
{
    QString id = QString::fromStdString(user.ID);
    Server::instance().addChatTo(id, chatID);
    proxy = Server::instance().getClientCallback(id);
}

void LocalParticipant::invite(const QStringList &users, const sdc::ByteSeq &sessionKey)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    proxy->initChat(sdc::sdcHelper::qStringListToStringSeq(users), chatID.toStdString(), sessionKey);
}

void LocalParticipant::addChatParticipant(const sdc::User &participant)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    proxy->addChatParticipant(participant, chatID.toStdString());
}

void LocalParticipant::removeChatParticipant(const sdc::User &participant)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        proxy->removeChatParticipant(participant, chatID.toStdString());
    } catch (const sdc::ParticipationException &e) {
        QLOG_ERROR() << "A user could not be notified that a chat participant left";
    }
}

void LocalParticipant::appendMessageToChat(const sdc::User &user, const sdc::ByteSeq &message)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    proxy->appendMessageToChat(message, chatID.toStdString(), user);
}

RemoteParticipant::RemoteParticipant(const sdc::User &user, const QString &chatID)
    : Participant(user, chatID)
{
    QString hostname = QString::fromStdString(sdc::sdcHelper::getServerFromID(user.ID));
    proxy = Server::instance().getInterServerProxy(hostname);
}

void RemoteParticipant::invite(const QStringList &users, const sdc::ByteSeq &sessionKey)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    proxy->clientInitChat(self, sdc::sdcHelper::qStringListToStringSeq(users), chatID.toStdString(), sessionKey);
}

void RemoteParticipant::addChatParticipant(const sdc::User &participant)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        proxy->clientAddChatParticipant(self, participant, chatID.toStdString());
    } catch (const sdc::ParticipationException &e) {
        QLOG_ERROR() << "A user could not be notified of a new chat participant.";
    }
}

void RemoteParticipant::removeChatParticipant(const sdc::User &participant)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        proxy->clientRemoveChatParticipant(self, participant, chatID.toStdString());
    } catch (const sdc::ParticipationException &e) {
        QLOG_ERROR() << "A user could not be notified that a chat participant left";
    }
}

void RemoteParticipant::appendMessageToChat(const sdc::User &user, const sdc::ByteSeq &message)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        proxy->clientAppendMessageToChat(self, message, chatID.toStdString(), user);
    } catch (const sdc::MessageCallbackException &e) {
        QLOG_ERROR() << "A user could not be notified of a new message in a chat";
    }
}

}

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


#include "chat.h"

#include <QMutexLocker>

#include "QsLog.h"
#include "server.h"
#include "sdcHelper.h"

namespace sdcs
{

Chat::Chat(const QString &chatID)
    : chatID(chatID)
{
}

Chat::~Chat()
{
}

QString Chat::getChatID() const
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    return chatID;
}

LocalChat::LocalChat(const QString &chatID, const sdc::User &user, sdc::ChatClientCallbackIPrx callback)
    : Chat(chatID)
{
    /* Add the user opening the chat to the participants list. */

    QSharedPointer<Participant> p(new LocalParticipant(user, callback, chatID));
    QString username = QString::fromStdString(user.ID);

    participants[username] = p;
}

void LocalChat::appendMessageFrom(const sdc::User &user, const sdc::ByteSeq &message)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QMutexLocker locker(&participantsMutex);
    QList<QSharedPointer<Participant> > keys = participants.values();
    locker.unlock();

    // If we can't deliver the message to some of the users,
    // we can report that, but may still want to reach the others
    QStringList failures;

    for (int i = 0; i < keys.size(); i++) {

        QSharedPointer<Participant> p = keys[i];

        try {
            p->appendMessageToChat(user, message);
        } catch (sdc::MessageCallbackException) {
            failures << p->getUserID();
            // TODO: Would it be sensible to remove these users
            // from the chat?
        }
    }

    if (!failures.isEmpty()) {
        QString errmsg = "Could not deliver the message to some users: %1;";
        QLOG_ERROR() << errmsg.arg(failures.join(" "));
    }
}

void LocalChat::inviteUser(const sdc::User &user, const sdc::ByteSeq &sessionKey)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<Participant> p;

    QString hostname = QString::fromStdString(sdc::sdcHelper::getServerFromID(user.ID));
    QString username = QString::fromStdString(user.ID);

    QMutexLocker locker(&participantsMutex);

    /* Prevent inviting a user that is already in the chat. */
    if (participants.contains(username)) {
        throw sdc::UserHandlingException("User is already in chat");
    }

    if (hostname == Server::instance().getHostname()) {
        p = QSharedPointer<Participant>(new LocalParticipant(user, chatID));
    } else {
        p = QSharedPointer<Participant>(new RemoteParticipant(user, chatID));
    }

    p->invite(participants.keys(), sessionKey);

    QList<QSharedPointer<Participant> > values = participants.values();
    for (int i = 0; i < values.size(); i++) {
        QSharedPointer<Participant> p = values[i];
        p->addChatParticipant(user);
    }

    participants[username] = p;
    locker.unlock();
}

void LocalChat::leaveChat(const sdc::User &user)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QString username = QString::fromStdString(user.ID);

    QMutexLocker locker(&participantsMutex);
    if (!participants.contains(username))
        throw sdc::SessionException("User is not in the chat.");

    participants.remove(username);

    QList<QSharedPointer<Participant> > values = participants.values();
    for (int i = 0; i < values.size(); i++) {
        QSharedPointer<Participant> p = values[i];
        p->removeChatParticipant(user);
    }

    locker.unlock();

    if (participants.empty())
        Server::instance().removeChat(chatID);
}

RemoteChat::RemoteChat(const QString &chatID) : Chat(chatID)
{
    QString hostname = QString::fromStdString(sdc::sdcHelper::getServerFromID(
                           chatID.toStdString()));
    interServer = Server::instance().getInterServerProxy(hostname);
}

void RemoteChat::appendMessageFrom(const sdc::User &user, const sdc::ByteSeq &message)
{
    // Can throw InterServerExeption, which is also expected in SessionI
    interServer->sendMessage(user, message, chatID.toStdString());
}

void RemoteChat::inviteUser(const sdc::User &user, const sdc::ByteSeq &sessionKey)
{
    // Can throw UserHandlingException and InterServerException, which is also
    // expected in SessionI
    interServer->invite(user, chatID.toStdString(), sessionKey);
}

void RemoteChat::leaveChat(const sdc::User &user)
{
    // Again, can throw InterServerException, which is expected in SessionI
    interServer->leaveChat(user, chatID.toStdString());
}

}

#include "chat.h"

#include <QMutexLocker>

#include "QsLog.h"
#include "server.h"

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

void Chat::notifyJoin(const sdc::User &joined)
{
    QMutexLocker locker(&participantsMutex);
    QList<QSharedPointer<Participant> > keys = participants.values();
    locker.unlock();

    for (int i = 0; i < keys.size(); i++) {
        QSharedPointer<Participant> p = keys[i];
        p->addChatParticipant(joined);
    }
}

void Chat::notifyLeave(const sdc::User &left)
{
    QMutexLocker locker(&participantsMutex);
    QList<QSharedPointer<Participant> > keys = participants.values();
    locker.unlock();

    for (int i = 0; i < keys.size(); i++) {
        QSharedPointer<Participant> p = keys[i];
        p->removeChatParticipant(left);
    }
}

LocalChat::LocalChat(const QString &chatID, const sdc::User &user, sdc::ChatClientCallbackIPrx callback)
    : Chat(chatID)
{
    /* Add the user opening the chat to the participants list. */

    QSharedPointer<Participant> p(new LocalParticipant(user, callback, chatID));
    QString username = QString::fromStdString(user.ID);

    participants[username] = p;

    notifyJoin(user);
}

void LocalChat::appendMessageFrom(const sdc::User &user, const sdc::ByteSeq &message)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QMutexLocker locker(&participantsMutex);
    QList<QSharedPointer<Participant> > keys = participants.values();
    locker.unlock();

    for (int i = 0; i < keys.size(); i++) {
        QSharedPointer<Participant> p = keys[i];
        p->appendMessageToChat(user, message);
    }
}

void LocalChat::inviteUser(const sdc::User &user, const sdc::ByteSeq &sessionKey)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<Participant> p(new LocalParticipant(user, chatID));
    QString username = QString::fromStdString(user.ID);

    QMutexLocker locker(&participantsMutex);
    p->invite(participants.keys(), sessionKey);
    participants[username] = p;
    locker.unlock();

    notifyJoin(user);
}

void LocalChat::leaveChat(const sdc::User &user)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QString username = QString::fromStdString(user.ID);

    QMutexLocker locker(&participantsMutex);
    if (!participants.contains(username))
        throw sdc::SessionException("User is not in the chat.");

    participants.remove(username);
    locker.unlock();

    notifyLeave(user);

    if (participants.empty())
        Server::instance().removeChat(chatID);
}

}

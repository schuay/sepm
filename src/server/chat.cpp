#include "chat.h"

#include <QMutexLocker>

#include "QsLog.h"

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

void Chat::notifyAll(const sdc::User &joined)
{
    QMutexLocker locker(&participantsMutex);
    QList<QSharedPointer<Participant> > keys = participants.values();
    locker.unlock();

    for (int i = 0; i < keys.size(); i++) {
        QSharedPointer<Participant> p = keys[i];
        p->addChatParticipant(joined);
    }
}

LocalChat::LocalChat(const QString &chatID, const sdc::User &user, sdc::ChatClientCallbackIPrx callback)
    : Chat(chatID)
{
    /* Add the user opening the chat to the participants list. */

    QSharedPointer<Participant> p(new LocalParticipant(user, callback, chatID));
    QString username = QString::fromStdString(user.ID);

    participants[username] = p;

    notifyAll(user);
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

    notifyAll(user);
}

void LocalChat::leaveChat(const QString &/*user*/)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

}

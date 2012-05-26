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

LocalChat::LocalChat(const QString &chatID, const sdc::User &user)
    : Chat(chatID)
{
    /* Add the user opening the chat to the participants list. */

    QSharedPointer<Participant> p(new LocalParticipant(user, chatID));
    QString username = QString::fromStdString(user.ID);

    QMutexLocker locker(&participantsMutex);
    participants[username] = p;
}

void LocalChat::appendMessageFrom(const sdc::User &/*user*/, const sdc::ByteSeq &/*message*/)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void LocalChat::inviteUser(const sdc::User &/*user*/, const sdc::ByteSeq &/*message*/)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void LocalChat::leaveChat(const QString &/*user*/)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

}

#include "chat.h"

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

LocalChat::LocalChat(const QString &chatID)
    : Chat(chatID)
{
}

void LocalChat::appendMessageFrom(sdc::User /*user*/, sdc::ByteSeq /*message*/)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void LocalChat::inviteUser(const Participant &/*user*/, sdc::ByteSeq /*sessionKey*/)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void LocalChat::leaveChat(const Participant &/*user*/)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

}

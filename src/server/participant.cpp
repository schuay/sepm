#include "participant.h"

#include "QsLog.h"
#include "server.h"

namespace sdcs
{

Participant::Participant(const sdc::User &user, const QString &chatID)
    : self(user), chatID(chatID)
{
}

Participant::~Participant()
{
}

LocalParticipant::LocalParticipant(const sdc::User &user, const QString &chatID)
    : Participant(user, chatID)
{
    proxy = Server::instance().getCallback(QString::fromStdString(user.ID));
}

void LocalParticipant::invite(QStringList /*users*/, sdc::ByteSeq /*sessionKey*/)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    /* TODO: implementation. */
}

void LocalParticipant::addChatParticipant(const sdc::User &participant)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    proxy->addChatParticipant(participant, chatID.toStdString());
}

void LocalParticipant::removeChatParticipant(const sdc::User &participant)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    proxy->removeChatParticipant(participant, chatID.toStdString());
}

void LocalParticipant::appendMessageToChat(const sdc::User &user, const sdc::ByteSeq &message)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    proxy->appendMessageToChat(message, chatID.toStdString(), user);
}


}

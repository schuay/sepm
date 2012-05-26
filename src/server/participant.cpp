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


LocalParticipant::LocalParticipant(const sdc::User &user, sdc::ChatClientCallbackIPrx callback,
                                   const QString &chatID)
    : Participant(user, chatID)
{
    proxy = callback;
}

LocalParticipant::LocalParticipant(const sdc::User &user, const QString &chatID)
    : Participant(user, chatID)
{
    proxy = Server::instance().addChatTo(QString::fromStdString(user.ID), chatID);
}

void LocalParticipant::invite(const QStringList &users, const sdc::ByteSeq &sessionKey)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    sdc::StringSeq seq;
    for (int i = 0; i < users.size(); i++) {
        seq.push_back(users[i].toStdString());
    }

    proxy->initChat(seq, chatID.toStdString(), sessionKey);
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

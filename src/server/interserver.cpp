#include "interserver.h"

#include "QsLog.h"

namespace sdcs
{

sdc::User InterServer::retrieveUser(const std::string &/*userID*/, const Ice::Current &)
throw(sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    sdc::User ret;
    return ret;
}

void InterServer::invite(const sdc::User &/*participant*/,
                         const std::string &/*chatID*/,
                         const sdc::ByteSeq &/*key*/,
                         const Ice::Current &)
throw(sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void InterServer::sendMessage(const sdc::User &/*sender*/,
                              const sdc::ByteSeq &/*message*/,
                              const std::string &/*chatID*/,
                              const Ice::Current &)
throw(sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void InterServer::leaveChat(const sdc::User &/*participant*/,
                            const std::string &/*chatID*/,
                            const Ice::Current &)
throw(sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void InterServer::clientInitChat(const sdc::User &/*client*/,
                                 const sdc::StringSeq &/*users*/,
                                 const std::string &/*chatID*/,
                                 const sdc::ByteSeq &/*key*/,
                                 const Ice::Current &)
throw(sdc::ChatException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void InterServer::clientAddChatParticipant(
    const sdc::User &/*client*/,
    const sdc::User &/*participant*/,
    const std::string &/*chatID*/,
    const Ice::Current &)
throw(sdc::ParticipationException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void InterServer::clientRemoveChatParticipant(
    const sdc::User &/*client*/,
    const sdc::User &/*participant*/,
    const std::string &/*chatID*/,
    const Ice::Current &)
throw(sdc::ParticipationException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void InterServer::clientAppendMessageToChat(
    const sdc::User &/*client*/,
    const sdc::ByteSeq &/*message*/,
    const std::string &/*chatID*/,
    const sdc::User &/*participant*/,
    const Ice::Current &)
throw(sdc::MessageCallbackException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

}

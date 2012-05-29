#include "interserver.h"

#include "QsLog.h"
#include "sdcHelper.h"
#include "userdbproxy.h"
#include "server.h"

namespace sdcs
{

sdc::User InterServer::retrieveUser(const std::string &userID, const Ice::Current &)
throw(sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QString hostname = QString::fromStdString(sdc::sdcHelper::getServerFromID(userID));

    if (hostname != Server::instance().getHostname())
        throw sdc::InterServerException("User is not registered on this server");

    try {
        QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(QString::fromStdString(userID));
        return proxy->getUser();
    } catch (const sdc::UserHandlingException &e) {
        throw sdc::InterServerException(e.what);
    }
}

void InterServer::invite(const sdc::User &participant,
                         const std::string &chatID,
                         const sdc::ByteSeq &key,
                         const Ice::Current &)
throw(sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        QSharedPointer<Chat> p = Server::instance().getChat(QString::fromStdString(chatID));
        p->inviteUser(participant, key);
    } catch (const sdc::InterServerException &e) {
        // getChat throws this with a useful message
        throw e;
    } catch (const sdc::ChatException &e) {
        // thrown by the Users' ChatClientCallback
        throw sdc::InterServerException("Received error from client " + participant.ID + ":\n" + e.what);
    } catch (...) {
        throw sdc::InterServerException();
    }
}

void InterServer::sendMessage(const sdc::User &sender,
                              const sdc::ByteSeq &message,
                              const std::string &chatID,
                              const Ice::Current &)
throw(sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        QSharedPointer<Chat> p = Server::instance().getChat(QString::fromStdString(chatID));
        p->appendMessageFrom(sender, message);
    } catch (const sdc::MessageCallbackException &e) {
        // chat throws this with a useful message
        throw sdc::InterServerException(e.what);
    } catch (...) {
        throw sdc::InterServerException();
    }
}

void InterServer::leaveChat(const sdc::User &participant,
                            const std::string &chatID,
                            const Ice::Current &)
throw(sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        QSharedPointer<Chat> p = Server::instance().getChat(QString::fromStdString(chatID));
        p->leaveChat(participant);
    } catch (...) {
        // Exception should not normally occur here, since these are caught
        // in participant and logged as an error.
        throw sdc::InterServerException();
    }
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
    const sdc::User &client,
    const sdc::ByteSeq &message,
    const std::string &chatID,
    const sdc::User &participant,
    const Ice::Current &)
throw(sdc::MessageCallbackException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QString id = QString::fromStdString(client.ID);

    try {
        sdc::ChatClientCallbackIPrx proxy = Server::instance().getClientCallback(id);
        proxy->appendMessageToChat(message, chatID, participant);
    } catch (...) {
        throw sdc::MessageCallbackException();
    }
}

}

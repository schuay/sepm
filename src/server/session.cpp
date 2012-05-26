#include "session.h"

#include <QMutexLocker>

#include "userdbproxy.h"
#include "QsLog.h"
#include "server.h"

namespace sdcs
{

Session::Session(const sdc::User &user, sdc::ChatClientCallbackIPrx callback, Server *server)
    : user(user), callback(callback), server(server)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

/* destroy self, notify all open chats */
void Session::logout(const Ice::Current &) throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    server->removeSession(QString::fromStdString(user.ID));
}

/* local user: db lookup
 * remote: create interserver, forward call, return. */
sdc::User Session::retrieveUser(const std::string &userID, const Ice::Current &)
throw(sdc::UserHandlingException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(QString::fromStdString(userID));

    return proxy->getUser();
}

std::string Session::initChat(const Ice::Current &) throw(sdc::SessionException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<Chat> p = server->createLocalChat();
    QString chatID = p->getChatID();

    QMutexLocker locker(&chatsMutex);
    chats[chatID] = p;

    return chatID.toStdString();
}

/* notify chat, remove from local list */
void Session::leaveChat(const std::string &/*chatID*/, const Ice::Current &)
throw(sdc::SessionException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

/* check consistency (chat exists, chat joined)
 * create participant (remote or local), call invite. */
void Session::invite(const sdc::User &/*participant*/, const std::string &/*chatID*/,
                     const sdc::ByteSeq &/*sessionKey*/, const Ice::Current &)
throw(sdc::UserHandlingException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

/* looks up chat and calls appendMessageFrom. */
void Session::sendMessage(const sdc::ByteSeq &/*message*/, const std::string &/*chatID*/, const Ice::Current &)
throw(sdc::MessageException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

/* check auth, do db ops, logout (make sure everything is consistent), destroy session */
void Session::deleteUser(const sdc::User &participant, const Ice::Current &current)
throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    if (participant != user)
        throw sdc::UserHandlingException("Unauthorized deletion attempt");

    QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                            QString::fromStdString(participant.ID));
    proxy->deleteUser();

    logout(current);
}

void Session::saveLog(const std::string &chatID, Ice::Long timestamp,
                      const sdc::SecureContainer &log, const Ice::Current &)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                            QString::fromStdString(user.ID));
    proxy->saveLog(QString::fromStdString(chatID), static_cast<long>(timestamp), log);
}

sdc::Loglist Session::retrieveLoglist(const Ice::Current &) throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                            QString::fromStdString(user.ID));
    return proxy->retrieveLoglist();
}

sdc::SecureContainer Session::retrieveLog(const std::string &chatID,
        Ice::Long timestamp, const Ice::Current &)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                            QString::fromStdString(user.ID));
    return proxy->retrieveLog(QString::fromStdString(chatID), static_cast<long>(timestamp));
}

void Session::saveContactList(const sdc::SecureContainer &contactList, const Ice::Current &)
throw(sdc::ContactException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                            QString::fromStdString(user.ID));
    proxy->saveContactList(contactList);
}

sdc::SecureContainer Session::retrieveContactList(const Ice::Current &)
throw(sdc::ContactException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                            QString::fromStdString(user.ID));
    return proxy->retrieveContactList();
}

}

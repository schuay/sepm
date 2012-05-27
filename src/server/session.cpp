#include "session.h"

#include <QMutexLocker>

#include "userdbproxy.h"
#include "QsLog.h"
#include "server.h"

namespace sdcs
{

Session::Session(const sdc::User &user, sdc::ChatClientCallbackIPrx callback)
    : self(user), callback(callback)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void Session::logout(const Ice::Current &) throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    /* TODO: leave all open chats. */

    Server::instance().removeSession(QString::fromStdString(self.ID));
}

sdc::User Session::retrieveUser(const std::string &userID, const Ice::Current &)
throw(sdc::UserHandlingException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    /* TODO: handle interserver user requests. */

    QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(QString::fromStdString(userID));

    return proxy->getUser();
}

std::string Session::initChat(const Ice::Current &) throw(sdc::SessionException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<Chat> p = Server::instance().createLocalChat(self, callback);
    QString chatID = p->getChatID();

    QMutexLocker locker(&chatsMutex);
    chats[chatID] = p;

    return chatID.toStdString();
}

void Session::leaveChat(const std::string &chatID, const Ice::Current &)
throw(sdc::SessionException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    const QString id = QString::fromStdString(chatID);

    QMutexLocker locker(&chatsMutex);
    if (!chats.contains(id))
        throw sdc::SessionException("Chat does not exist");

    chats[id]->leaveChat(self);
    chats.remove(id);
}

void Session::invite(const sdc::User &participant, const std::string &chatID,
                     const sdc::ByteSeq &sessionKey, const Ice::Current &)
throw(sdc::UserHandlingException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    const QString id = QString::fromStdString(chatID);

    QMutexLocker locker(&chatsMutex);
    if (!chats.contains(id))
        throw sdc::SessionException("Chat does not exist");

    chats[id]->inviteUser(participant, sessionKey);
}

void Session::sendMessage(const sdc::ByteSeq &message, const std::string &chatID, const Ice::Current &)
throw(sdc::MessageException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    const QString id = QString::fromStdString(chatID);

    QMutexLocker locker(&chatsMutex);
    if (!chats.contains(id))
        throw sdc::MessageException("Chat does not exist");

    chats[id]->appendMessageFrom(self, message);
}

void Session::deleteUser(const sdc::User &participant, const Ice::Current &current)
throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    if (participant != self)
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
                                            QString::fromStdString(self.ID));
    proxy->saveLog(QString::fromStdString(chatID), static_cast<long>(timestamp), log);
}

sdc::Loglist Session::retrieveLoglist(const Ice::Current &) throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                            QString::fromStdString(self.ID));
    return proxy->retrieveLoglist();
}

sdc::SecureContainer Session::retrieveLog(const std::string &chatID,
        Ice::Long timestamp, const Ice::Current &)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                            QString::fromStdString(self.ID));
    return proxy->retrieveLog(QString::fromStdString(chatID), static_cast<long>(timestamp));
}

void Session::saveContactList(const sdc::SecureContainer &contactList, const Ice::Current &)
throw(sdc::ContactException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                            QString::fromStdString(self.ID));
    proxy->saveContactList(contactList);
}

sdc::SecureContainer Session::retrieveContactList(const Ice::Current &)
throw(sdc::ContactException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                            QString::fromStdString(self.ID));
    return proxy->retrieveContactList();
}

sdc::ChatClientCallbackIPrx Session::addChat(QSharedPointer<Chat> chat)
{
    QMutexLocker locker(&chatsMutex);
    if (chats.contains(chat->getChatID())) {
        throw sdc::UserHandlingException("User is already in chat.");
    }
    chats[chat->getChatID()] = chat;

    return callback;
}

}

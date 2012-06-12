#include "session.h"

#include <QMutexLocker>

#include "userdbproxy.h"
#include "QsLog.h"
#include "server.h"
#include "sdcHelper.h"

namespace sdcs
{

#define UNEXPECTED_EXCEPTION(type) type("Unexpected exception in: " __PRETTY_FUNCTION)

static inline bool isLocal(const QString &id)
{
    return (QString::fromStdString(sdc::sdcHelper::getServerFromID(id.toStdString()))
            == Server::instance().getHostname());
}

Session::Session(const sdc::User &user, sdc::ChatClientCallbackIPrx callback)
    : self(user), callback(callback)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void Session::logout(const Ice::Current &) throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        QMutexLocker locker(&chatsMutex);
        QList<QSharedPointer<Chat> > values = chats.values();
        for (int i = 0; i < values.size(); i++) {
            try {
                values[i]->leaveChat(self);
            } catch (const Ice::Exception &) {
                /* Don't bother the client with this. */
                QLOG_INFO() << __PRETTY_FUNCTION__
                            << ": Lost connection to remote server, cannot leave remote chat";
            }
        }

        Server::instance().removeSession(QString::fromStdString(self.ID));

    } catch (const sdc::UserHandlingException &e) {
        throw sdc::UserHandlingException("Internal Server Error: "
                                         "Session is connected but unknown to server.");
    } catch (...) {
        throw sdc::UserHandlingException("Unexpected error");
    }
}

sdc::User Session::retrieveUser(const std::string &userID, const Ice::Current &)
throw(sdc::UserHandlingException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QString hostname = QString::fromStdString(sdc::sdcHelper::getServerFromID(userID));

    if (hostname == Server::instance().getHostname()) {
        try {
            QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(QString::fromStdString(userID));
            return proxy->getUser();
        } catch (const sdc::UserHandlingException &e) {
            throw e;
        } catch (...) {
            throw sdc::UserHandlingException();
        }
    } else {
        try {
            sdc::InterServerIPrx proxy = Server::instance().getInterServerProxy(hostname);
            sdc::User u = proxy->retrieveUser(userID);

            /* The reference server is bugged and returns an ID without hostname. */
            QString id = QString::fromStdString(u.ID);
            if (!id.contains('@')) {
                id = id + "@" + hostname;
                u.ID = id.toStdString();
            }

            return u;
        } catch (const sdc::InterServerException &e) {
            throw e;
        } catch (const sdc::UserHandlingException &e) {
            throw sdc::InterServerException(e.what);
        } catch (...) {
            throw sdc::InterServerException("Inter-Server communication failed");
        }
    }
}

std::string Session::initChat(const Ice::Current &) throw(sdc::SessionException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        QSharedPointer<Chat> p = Server::instance().createLocalChat(self, callback);
        QString chatID = p->getChatID();

        QMutexLocker locker(&chatsMutex);
        chats[chatID] = p;

        return chatID.toStdString();
    } catch (const sdc::SessionException &e) {
        throw e;
    } catch (...) {
        throw sdc::SessionException();
    }
}

void Session::leaveChat(const std::string &chatID, const Ice::Current &)
throw(sdc::SessionException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    const QString id = QString::fromStdString(chatID);

    try {
        QMutexLocker locker(&chatsMutex);
        if (!chats.contains(id))
            throw sdc::SessionException("Chat does not exist");

        chats[id]->leaveChat(self);
        chats.remove(id);
    } catch (const sdc::SessionException &e) {
        throw e;
    } catch (const sdc::InterServerException &e) {
        throw e;
    } catch (...) {
        if (isLocal(id))
            throw sdc::SessionException();
        else
            throw sdc::InterServerException();
    }
}

void Session::invite(const sdc::User &participant, const std::string &chatID,
                     const sdc::ByteSeq &sessionKey, const Ice::Current &)
throw(sdc::UserHandlingException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    const QString id = QString::fromStdString(chatID);

    try {
        QMutexLocker locker(&chatsMutex);
        if (!chats.contains(id))
            throw sdc::SessionException("Chat does not exist");

        chats[id]->inviteUser(participant, sessionKey);
    } catch (const sdc::UserHandlingException &e) {
        throw e;
    } catch (const sdc::InterServerException &e) {
        throw e;
    } catch (...) {
        if (isLocal(id))
            throw sdc::UserHandlingException();
        else
            throw sdc::InterServerException();
    }
}

void Session::sendMessage(const sdc::ByteSeq &message, const std::string &chatID, const Ice::Current &)
throw(sdc::MessageException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    const QString id = QString::fromStdString(chatID);

    try {
        QMutexLocker locker(&chatsMutex);
        if (!chats.contains(id))
            throw sdc::MessageException("Chat does not exist");

        chats[id]->appendMessageFrom(self, message);
    } catch (const sdc::MessageException &e) {
        throw e;
    } catch (const sdc::InterServerException &e) {
        throw e;
    } catch (...) {
        if (isLocal(id))
            throw sdc::MessageException();
        else
            throw sdc::InterServerException();
    }
}

void Session::deleteUser(const sdc::User &participant, const Ice::Current &current)
throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        if (participant != self)
            throw sdc::UserHandlingException("Unauthorized deletion attempt");

        QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                                QString::fromStdString(participant.ID));
        proxy->deleteUser();

        logout(current);
    } catch (const sdc::UserHandlingException &e) {
        throw e;
    } catch (...) {
        throw sdc::UserHandlingException();
    }
}

void Session::saveLog(const std::string &chatID, Ice::Long timestamp,
                      const sdc::SecureContainer &log, const Ice::Current &)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                                QString::fromStdString(self.ID));
        proxy->saveLog(QString::fromStdString(chatID), static_cast<long>(timestamp), log);
    } catch (const sdc::LogException &e) {
        throw e;
    } catch (...) {
        throw sdc::LogException();
    }
}

sdc::Loglist Session::retrieveLoglist(const Ice::Current &) throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                                QString::fromStdString(self.ID));
        return proxy->retrieveLoglist();
    } catch (const sdc::LogException &e) {
        throw e;
    } catch (...) {
        throw sdc::LogException();
    }
}

sdc::SecureContainer Session::retrieveLog(const std::string &chatID,
        Ice::Long timestamp, const Ice::Current &)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                                QString::fromStdString(self.ID));
        return proxy->retrieveLog(QString::fromStdString(chatID), static_cast<long>(timestamp));
    } catch (const sdc::LogException &e) {
        throw e;
    } catch (...) {
        throw sdc::LogException();
    }
}

void Session::saveContactList(const sdc::SecureContainer &contactList, const Ice::Current &)
throw(sdc::ContactException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                                QString::fromStdString(self.ID));
        proxy->saveContactList(contactList);
    } catch (const sdc::ContactException &e) {
        throw e;
    } catch (...) {
        throw sdc::ContactException();
    }
}

sdc::SecureContainer Session::retrieveContactList(const Ice::Current &)
throw(sdc::ContactException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    try {
        QSharedPointer<UserDbProxy> proxy = UserDbProxy::getProxy(
                                                QString::fromStdString(self.ID));
        return proxy->retrieveContactList();
    } catch (const sdc::ContactException &e) {
        throw e;
    } catch (...) {
        throw sdc::ContactException();
    }
}

void Session::addChat(QSharedPointer<Chat> chat)
{
    QMutexLocker locker(&chatsMutex);
    if (chats.contains(chat->getChatID())) {
        throw sdc::UserHandlingException("User is already in chat.");
    }
    chats[chat->getChatID()] = chat;
}

sdc::ChatClientCallbackIPrx Session::getCallback()
{
    return callback;
}

}

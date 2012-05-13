#include "session.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QtConcurrentRun>
#include <QMutexLocker>

#include "QsLog.h"

#include "Security.h"

namespace sdcc
{

struct SessionPrivate : public sdc::ChatClientCallbackI {
    SessionPrivate(Session *q, Ice::CommunicatorPtr c)
        : communicator(c), q_ptr(q) { }

    void initChat(const sdc::StringSeq &cUsers, const std::string &chatID,
                  const sdc::ByteSeq &sessionKey, const Ice::Current &) {
        QLOG_TRACE() << __PRETTY_FUNCTION__;
        Q_Q(Session);

        QString key = QString::fromStdString(chatID);
        QSharedPointer<Chat> cp;

        QMutexLocker locker(&chatsMutex);
        if (chats.contains(key)) {
            QLOG_WARN() << QString("Received invitation for a chat we are already in ('%1')")
                        .arg(key);
            return;
        }

        cp = QSharedPointer<Chat>(new Chat(session, *q, key, sessionKey));

        sdc::StringSeq::const_iterator i;
        cp->addChatParticipant(user);
        try {
            for (i = cUsers.begin(); i < cUsers.end(); i++) {
                cp->addChatParticipant(getUser(QString::fromStdString(*i)));
            }

            chats[key] = cp;
            emit q->invitationReceived(cp);
        } catch (const sdc::UserHandlingException &e) {
            QLOG_ERROR() << QString("Received invitation with invalid user, '%1', '%2'")
                         .arg(key).arg(QString::fromStdString(*i));
        } catch (const sdc::InterServerException &e) {
            QLOG_ERROR() << QString("Received invitation with invalid user, '%1', '%2'")
                         .arg(key).arg(QString::fromStdString(*i));
        }
    }

    void addChatParticipant(const sdc::User &participant,
                            const std::string &chatID, const Ice::Current &) {
        QLOG_TRACE() << __PRETTY_FUNCTION__;

        QSharedPointer<User> usr(new User(participant));
        QString key = QString::fromStdString(chatID);

        QMutexLocker chatLocker(&chatsMutex);
        if (!chats.contains(key)) {
            QLOG_ERROR() << QString("New participant for nonexistant chat '%1'").arg(key);
            return;
        }

        QMutexLocker userLocker(&usersMutex);
        users[usr->getName()] = usr;

        chats[key]->addChatParticipant(usr);

        emit chats[key]->userJoined(*usr.data());
    }

    void removeChatParticipant(const sdc::User &/*participant*/,
                               const std::string &/*chatID*/, const Ice::Current &) {
        QLOG_TRACE() << __PRETTY_FUNCTION__;
    }

    void appendMessageToChat(const sdc::ByteSeq &message, const std::string &chatID,
                             const sdc::User &participant, const Ice::Current &) {

        QLOG_TRACE() << __PRETTY_FUNCTION__;

        QSharedPointer<User> usr(new User(participant));

        QMutexLocker chatLocker(&chatsMutex);
        QString key = QString::fromStdString(chatID);
        if (!chats.contains(key)) {
            QLOG_ERROR() << QString("Received message for nonexistant chat '%1'").arg(key);
            return;
        }

        QMutexLocker userLocker(&usersMutex);
        users[usr->getName()] = usr;

        chats[key]->receiveMessage(*usr.data(), message);
    }

    std::string echo(const std::string &message, const Ice::Current &) {
        QLOG_TRACE() << __PRETTY_FUNCTION__;
        return message;
    }

    void runRetrieveUser(const QString &username) {
        Q_Q(Session);
        bool success = true;
        QString message;
        QSharedPointer<User> usr;

        try {
            QMutexLocker locker(&chatsMutex);
            usr = retrieveUser(username);
        } catch (const sdc::UserHandlingException &e) {
            success = false;
            message = e.what.c_str();
        } catch (const sdc::InterServerException &e) {
            success = false;
            message = e.what.c_str();
        }

        emit q->retrieveUserCompleted(usr, success, message);
    }

    void runInitChat() {
        Q_Q(Session);
        bool success = true;
        QString message;
        QSharedPointer<Chat> cp;

        try {
            QString key = QString::fromStdString(session->initChat());
            sdc::Security s;
            cp = QSharedPointer<Chat>(new Chat(session, *q, key,
                                               s.genAESKey(SESSION_KEY_SIZE)));

            QMutexLocker locker(&chatsMutex);
            chats[key] = cp;

            cp->addChatParticipant(user);
        } catch (const sdc::SessionException &e) {
            success = false;
            message = e.what.c_str();
        } catch (const sdc::SecurityException &e) {
            success = false;
            message = e.what();
        }

        emit q->initChatCompleted(cp, success, message);
    }

    void runLogout() {
        Q_Q(Session);
        bool success = true;
        QString message;

        try {
            session->logout();
        } catch (const sdc::UserHandlingException &e) {
            success = false;
            message = e.what.c_str();
        }

        emit q->logoutCompleted(success, message);
    }

    void runDeleteUser(const User &user) {
        Q_Q(Session);
        bool success = true;
        QString message;

        try {
            session->deleteUser(*user.getIceUser().data());
        } catch (const sdc::UserHandlingException &e) {
            success = false;
            message = e.what.c_str();
        }

        emit q->deleteUserCompleted(success, message);
    }

    ~SessionPrivate() {
        if (communicator) {
            communicator->destroy();
        }
    }

    QMutex chatsMutex;
    QMutex usersMutex;
    QMap<QString, QSharedPointer<Chat> > chats;
    QMap<QString, QSharedPointer<User> > users;

    Ice::CommunicatorPtr communicator;
    sdc::SessionIPrx session;

    QSharedPointer<User> user;

private:
    Session *q_ptr;
    Q_DECLARE_PUBLIC(Session)

    /* Internal helper to get a User from the server and cache it. */
    QSharedPointer<User> retrieveUser(const QString &username) {
        QSharedPointer<User> usr(new User(session->retrieveUser(
                                              username.toStdString())));

        QMutexLocker locker(&usersMutex);
        users[username] = usr;
        return usr;
    }

    /* Internal helper to get a User from the local cache. The server is only
     * asked if the user is not in the cache. */
    QSharedPointer<User> getUser(const QString &username) {
        QMutexLocker locker(&usersMutex);
        if (!users.contains(username)) {
            locker.unlock();
            return retrieveUser(username);
        }

        return users[username];
    }

}; // struct SessionPrivate

const QSharedPointer<User> Session::getUser() const
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(const Session);

    return d->user;
}

Session::Session(const User &user, const QString &pwd, sdc::AuthenticationIPrx auth)
    : d_ptr(new SessionPrivate(this, auth->ice_getCommunicator()))
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);

    sdc::User sdcUser = *user.getIceUser().data();
    d->user = QSharedPointer<User> (new User(sdcUser));

    Ice::ConnectionPtr connection = auth->ice_getConnection();
    Ice::ObjectAdapterPtr adapter = connection->getAdapter();
    Ice::Identity identity = { IceUtil::generateUUID(), "" };

    if (!adapter) {
        adapter = d->communicator->createObjectAdapterWithEndpoints(
                      "ChatClientCallback", "default");
    }

    sdc::ChatClientCallbackIPtr callback(d);
    adapter->add(callback, identity);
    adapter->activate();
    auth->ice_getConnection()->setAdapter(adapter);

    d->session = auth->login(sdcUser, pwd.toStdString(), identity);

    if (!d->session)
        throw sdc::SDCException("Login failed");
}

void Session::initChat()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runInitChat);
}

void Session::logout()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runLogout);
}

void Session::deleteUser(const User &user)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runDeleteUser, user);
}

void Session::retrieveUser(const QString &username)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runRetrieveUser, username);
}

}

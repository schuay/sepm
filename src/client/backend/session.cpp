/* *************************************************************************
 *  Copyright 2012 TU VIENNA SEPM GROUP 22                                 *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */


#include "session.h"
#include "session_p.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QtConcurrentRun>
#include <QMutexLocker>

#include "QsLog.h"

namespace sdcc
{

void SessionPrivate::initChat(const sdc::StringSeq &cUsers,
                              const std::string &chatID,
                              const sdc::ByteSeq &sessionKeyEnc,
                              const Ice::Current &)
throw(sdc::ChatClientCallbackException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_Q(Session);

    QString key = QString::fromStdString(chatID);
    QSharedPointer<Chat> cp;

    sdc::ByteSeq sessionKey;
    try {
        sessionKey = user->decrypt(sessionKeyEnc);
    } catch (const sdc::SecurityException &e) {
        QString msg = QString("Could not decrypt session key for '%1'")
                      .arg(key);
        QLOG_ERROR() << msg;
        throw sdc::ChatClientCallbackException(msg.toStdString());
    }

    QMutexLocker locker(&chatsMutex);
    if (chats.contains(key)) {
        QString msg = QString("Received invitation for a chat we are already in ('%1')")
                      .arg(key);
        QLOG_ERROR() << msg;
        throw sdc::ChatClientCallbackException(msg.toStdString());
    }

    cp = QSharedPointer<Chat>(new Chat(session, *q, key, sessionKey));
    /* We need a Qt::DirectConnection here.
     * The default Qt::AutoConnection seems to believe that sender and
     * receiver are in a different thread and chooses Qt:QueuedConnection
     * which somehow doesn't work.
     *
     * see Bug #11396
     * see http://qt-project.org/doc/qt-4.8/qt.html#ConnectionType-enum
     */
    connect(cp.data(), SIGNAL(leaveChatCompleted(const QString)),
            this, SLOT(leaveChatCompletedSlot(const QString)),
            Qt::DirectConnection);

    sdc::StringSeq::const_iterator i;
    cp->addChatParticipant(user);
    try {
        for (i = cUsers.begin(); i < cUsers.end(); i++) {
            cp->addChatParticipant(getUser(QString::fromStdString(*i)));
        }

        chats[key] = cp;
        emit q->invitationReceived(cp);
    } catch (const sdc::UserHandlingException &e) {
        QString msg = QString("Received invitation with invalid user, '%1', '%2'")
                      .arg(key).arg(QString::fromStdString(*i));
        QLOG_ERROR() << msg;
        throw sdc::ChatClientCallbackException(msg.toStdString());
    } catch (const sdc::InterServerException &e) {
        QString msg = QString("Received invitation with invalid user, '%1', '%2'")
                      .arg(key).arg(QString::fromStdString(*i));
        QLOG_ERROR() << msg;
        throw sdc::ChatClientCallbackException(msg.toStdString());
    } catch (...) {
        throw sdc::ChatClientCallbackException("Unexpected exception");
    }
}

void SessionPrivate::addChatParticipant(const sdc::User &participant,
                                        const std::string &chatID,
                                        const Ice::Current &)
throw(sdc::ParticipationException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<const User> usr(new User(participant));
    QString key = QString::fromStdString(chatID);

    QMutexLocker chatLocker(&chatsMutex);
    if (!chats.contains(key)) {
        QString msg = QString("New participant for nonexistant chat '%1'")
                      .arg(key);
        QLOG_ERROR() << msg;
        throw sdc::ParticipationException(msg.toStdString());
    }

    QMutexLocker userLocker(&usersMutex);
    users[usr->getName()] = usr;

    chats[key]->addChatParticipant(usr);
}

void SessionPrivate::removeChatParticipant(const sdc::User &participant,
        const std::string &chatID,
        const Ice::Current &)
throw(sdc::ParticipationException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<const User> usr(new User(participant));
    QString key = QString::fromStdString(chatID);

    QMutexLocker chatLocker(&chatsMutex);
    if (!chats.contains(key)) {
        QString msg = QString("Remove participant from nonexistant chat '%1'")
                      .arg(key);
        QLOG_ERROR() << msg;
        throw sdc::ParticipationException(msg.toStdString());
    }

    QMutexLocker userLocker(&usersMutex);
    users[usr->getName()] = usr;

    chats[key]->removeChatParticipant(usr);
}

void SessionPrivate::appendMessageToChat(const sdc::ByteSeq &message,
        const std::string &chatID,
        const sdc::User &participant,
        const Ice::Current &)
throw(sdc::MessageCallbackException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<const User> usr(new User(participant));

    QMutexLocker chatLocker(&chatsMutex);
    QString key = QString::fromStdString(chatID);
    if (!chats.contains(key)) {
        QString msg = QString("Received message for nonexistant chat '%1'").arg(key);
        QLOG_ERROR() << msg;
        throw sdc::MessageCallbackException(msg.toStdString());
    }

    QMutexLocker userLocker(&usersMutex);
    users[usr->getName()] = usr;

    chats[key]->receiveMessage(usr, message);
}

std::string SessionPrivate::echo(const std::string &message, const Ice::Current &)
throw(sdc::SDCException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    return message;
}

void SessionPrivate::runRetrieveUser(const QString &username, const QObject *id)
{
    Q_Q(Session);
    bool success = true;
    QString message;
    QSharedPointer<const User> usr;

    try {
        QMutexLocker locker(&chatsMutex);
        usr = retrieveUser(username);
    } catch (const sdc::UserHandlingException &e) {
        success = false;
        message = e.what.c_str();
    } catch (const sdc::InterServerException &e) {
        success = false;
        message = e.what.c_str();
    } catch (...) {
        success = false;
        message = "Unexpected exception";
    }

    emit q->retrieveUserCompleted(usr, id, success, message);
}

void SessionPrivate::runInitChat()
{
    Q_Q(Session);
    bool success = true;
    QString message;
    QSharedPointer<Chat> cp;

    try {
        QString key = QString::fromStdString(session->initChat());
        sdc::Security s;
        cp = QSharedPointer<Chat>(new Chat(session, *q, key,
                                           s.genAESKey(SESSION_KEY_SIZE)));
        connect(cp.data(), SIGNAL(leaveChatCompleted(const QString)),
                this, SLOT(leaveChatCompletedSlot(const QString)));

        QMutexLocker locker(&chatsMutex);
        chats[key] = cp;

        cp->addChatParticipant(user);
    } catch (const sdc::SessionException &e) {
        success = false;
        message = e.what.c_str();
    } catch (const sdc::SecurityException &e) {
        success = false;
        message = e.what();
    } catch (...) {
        success = false;
        message = "Unexpected exception";
    }

    emit q->initChatCompleted(cp, success, message);
}

void SessionPrivate::leaveAllChats(void)
{
    QMutexLocker locker(&chatsMutex);
    QList<QSharedPointer<Chat> > chatList = chats.values();
    locker.unlock();

    for (int i = 0; i < chatList.size(); i++) {
        chatList[i]->runLeaveChat();
    }
}

void SessionPrivate::runLogout()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    Q_Q(Session);
    bool success = true;
    QString message;

    try {
        leaveAllChats();

        session->logout();
    } catch (const sdc::UserHandlingException &e) {
        success = false;
        message = e.what.c_str();
    } catch (...) {
        success = false;
        message = "Unexpected exception";
    }

    emit q->logoutCompleted(success, message);
}

void SessionPrivate::runDeleteUser(QSharedPointer<const User> user)
{
    Q_Q(Session);
    bool success = true;
    QString message;

    if (user->getName() == this->user->getName()) {
        leaveAllChats();
    }

    try {
        session->deleteUser(*user->getIceUser().data());
    } catch (const sdc::UserHandlingException &e) {
        success = false;
        message = e.what.c_str();
    } catch (...) {
        success = false;
        message = "Unexpected exception";
    }

    emit q->deleteUserCompleted(success, message);
}

void SessionPrivate::runRetrieveLoglist()
{
    Q_Q(Session);
    bool success = true;
    QString message;
    QList<QPair<QDateTime, QString> > loglistdata;

    try {
        QLOG_TRACE() << __PRETTY_FUNCTION__;

        sdc::Loglist list = session->retrieveLoglist();

        sdc::Loglist::const_iterator i;
        for (i = list.begin(); i < list.end(); i++) {
            loglistdata.append(QPair<QDateTime, QString>(QDateTime::fromTime_t(i->timestamp),
                               QString::fromStdString(i->chatID)));
        }

    } catch (const sdc::LogException &e) {
        success = false;
        message = e.what.c_str();
    } catch (...) {
        success = false;
        message = "Unexpected exception";
    }

    emit q->retrieveLoglistCompleted(loglistdata, success, message);
}

void SessionPrivate::runRetrieveLog(const QDateTime &time, const QString &chat)
{
    Q_Q(Session);
    bool success = true;
    QString message;
    QList<ChatlogEntry> loglistdata;

    try {
        sdc::SecureContainer loglist = session->retrieveLog(chat.toStdString(),
                                       time.toTime_t());
        if (loglist.data.size() <= 0) {
            goto out;
        }

        QLOG_TRACE() << __PRETTY_FUNCTION__;
        sdc::ByteSeq decryptedData = user->decrypt(loglist.data);

        if (!user->verify(decryptedData, loglist.signature)) {
            success = false;
            message = "Invalid signature";
            goto out;
        }

        sdc::Chatlog sdcLog;
        Ice::InputStreamPtr in = Ice::createInputStream(communicator, decryptedData);
        in->read(sdcLog);

        sdc::Chatlog::const_iterator i;
        for (i = sdcLog.begin(); i < sdcLog.end(); i++) {
            loglistdata.append(ChatlogEntry(QString::fromStdString(i->senderID),
                                            i->timestamp,
                                            QString::fromStdString(i->message)));
        }

    } catch (const sdc::LogException &e) {
        success = false;
        message = e.what.c_str();
    } catch (const sdc::SecurityException) {
        success = false;
        message = "Decryption Failed";
    } catch (...) {
        success = false;
        message = "Unexpected exception";
    }

out:
    emit q->retrieveLogCompleted(loglistdata, success, message);
}

void SessionPrivate::runRetrieveContactList()
{
    Q_Q(Session);
    bool success = true;
    QString message;
    QStringList contactlistdata;

    try {
        sdc::SecureContainer contactlist = session->retrieveContactList();
        if (contactlist.data.size() <= 0) {
            goto out;
        }

        QLOG_TRACE() << __PRETTY_FUNCTION__;
        sdc::ByteSeq decryptedData = user->decrypt(contactlist.data);

        if (!user->verify(decryptedData, contactlist.signature)) {
            success = false;
            message = "Invalid signature";
            goto out;
        }

        QByteArray b1 = sdc::sdcHelper::byteSeqToByteArray(decryptedData);
        if (b1.size() > 0) {
            contactlistdata = QString(b1).split('\n');
        }

    } catch (const sdc::ContactException &e) {
        success = false;
        message = e.what.c_str();
    } catch (const sdc::SecurityException) {
        success = false;
        message = "Decryption Failed";
    } catch (...) {
        success = false;
        message = "Unexpected exception";
    }

out:
    emit q->retrieveContactListCompleted(contactlistdata, success, message);
}

void SessionPrivate::runSaveContactList(const QStringList &contactlist)
{
    Q_Q(Session);
    bool success = true;
    QString message;

    try {
        sdc::SecureContainer contactListContainer;

        QByteArray temp;
        for (int i = 0; i < contactlist.size(); i++) {
            temp.append(contactlist[i] + '\n');
        }
        temp.chop(1);

        contactListContainer.data = user->encrypt(sdc::sdcHelper::byteArraytoByteSeq(temp));
        contactListContainer.signature = user->sign(sdc::sdcHelper::byteArraytoByteSeq(temp));

        session->saveContactList(contactListContainer);

    } catch (const sdc::ContactException &e) {
        success = false;
        message = e.what.c_str();
    } catch (...) {
        success = false;
        message = "Unexpected exception";
    }

    emit q->saveContactListCompleted(success, message);
}

SessionPrivate::~SessionPrivate()
{
    if (communicator) {
        communicator->destroy();
    }
}

/* Internal helper to get a User from the server and cache it. */
QSharedPointer<const User> SessionPrivate::retrieveUser(const QString &username)
{
    QSharedPointer<const User> usr(new User(session->retrieveUser(
            username.toStdString())));

    QMutexLocker locker(&usersMutex);
    users[username] = usr;
    return usr;
}

/* Internal helper to get a User from the local cache. The server is only
 * asked if the user is not in the cache. */
QSharedPointer<const User> SessionPrivate::getUser(const QString &username)
{
    QMutexLocker locker(&usersMutex);
    if (!users.contains(username)) {
        locker.unlock();
        return retrieveUser(username);
    }

    return users[username];
}

void SessionPrivate::leaveChatCompletedSlot(const QString &id)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QMutexLocker locker(&chatsMutex);
    chats.remove(id);
}

const QSharedPointer<const LoginUser> Session::getUser() const
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(const Session);

    return d->user;
}

Session::Session(QSharedPointer<const LoginUser> user, const QString &pwd,
                 sdc::AuthenticationIPrx auth)
    : d_ptr(new SessionPrivate(this, auth->ice_getCommunicator()))
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);

    sdc::User sdcUser = *user->getIceUser().data();
    d->user = user;

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

    d->valid = true;
}

bool Session::isValid() const
{
    Q_D(const Session);
    return d->valid;
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

    d->valid = false;

    QtConcurrent::run(d, &SessionPrivate::runLogout);
}

void Session::deleteUser(QSharedPointer<const User> user)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);

    if (user->getName() == d->user->getName()) {
        d->valid = false;
    }

    QtConcurrent::run(d, &SessionPrivate::runDeleteUser, user);
}

void Session::retrieveUser(const QString &username, const QObject *id)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runRetrieveUser, username, id);
}

void Session::retrieveLoglist()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runRetrieveLoglist);
}

void Session::retrieveLog(const QPair<QDateTime, QString> &spec)
{
    retrieveLog(spec.first, spec.second);
}

void Session::retrieveLog(const QDateTime &time, const QString &chat)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runRetrieveLog, time, chat);
}

void Session::retrieveContactList()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runRetrieveContactList);
}

void Session::saveContactList(const QStringList &contactlist)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runSaveContactList, contactlist);
}
}

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
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_Q(Session);

    QString key = QString::fromStdString(chatID);
    QSharedPointer<Chat> cp;

    sdc::ByteSeq sessionKey;
    try {
        sessionKey = user->decrypt(sessionKeyEnc);
    } catch (const sdc::SecurityException &e) {
        QLOG_ERROR() << QString("Could not decrypt session key for '%1'").arg(key);
        return;
    }

    QMutexLocker locker(&chatsMutex);
    if (chats.contains(key)) {
        QLOG_WARN() << QString("Received invitation for a chat we are already in ('%1')")
                    .arg(key);
        return;
    }

    cp = QSharedPointer<Chat>(new Chat(session, *q, key, sessionKey));
    connect(cp.data(), SIGNAL(leaveChatCompleted(bool, QString)),
            this, SLOT(leaveChatCompletedSlot(bool, QString)));

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

void SessionPrivate::addChatParticipant(const sdc::User &participant,
                                        const std::string &chatID,
                                        const Ice::Current &)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<const User> usr(new User(participant));
    QString key = QString::fromStdString(chatID);

    QMutexLocker chatLocker(&chatsMutex);
    if (!chats.contains(key)) {
        QLOG_ERROR() << QString("New participant for nonexistant chat '%1'").arg(key);
        return;
    }

    QMutexLocker userLocker(&usersMutex);
    users[usr->getName()] = usr;

    chats[key]->addChatParticipant(usr);
}

void SessionPrivate::removeChatParticipant(const sdc::User &participant,
        const std::string &chatID,
        const Ice::Current &)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<const User> usr(new User(participant));
    QString key = QString::fromStdString(chatID);

    QMutexLocker chatLocker(&chatsMutex);
    if (!chats.contains(key)) {
        QLOG_ERROR() << QString("Remove participant from nonexistant chat '%1'").arg(key);
        return;
    }

    QMutexLocker userLocker(&usersMutex);
    users[usr->getName()] = usr;

    chats[key]->removeChatParticipant(usr);
}

void SessionPrivate::appendMessageToChat(const sdc::ByteSeq &message,
        const std::string &chatID,
        const sdc::User &participant,
        const Ice::Current &)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSharedPointer<const User> usr(new User(participant));

    QMutexLocker chatLocker(&chatsMutex);
    QString key = QString::fromStdString(chatID);
    if (!chats.contains(key)) {
        QLOG_ERROR() << QString("Received message for nonexistant chat '%1'").arg(key);
        return;
    }

    QMutexLocker userLocker(&usersMutex);
    users[usr->getName()] = usr;

    chats[key]->receiveMessage(usr, message);
}

std::string SessionPrivate::echo(const std::string &message, const Ice::Current &)
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
        connect(cp.data(), SIGNAL(leaveChatCompleted(bool, QString)),
                this, SLOT(leaveChatCompletedSlot(bool, QString)));

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

void SessionPrivate::runLogout()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    Q_Q(Session);
    bool success = true;
    QString message;

    try {
        session->logout();

        QMutexLocker locker(&chatsMutex);
        QList<QSharedPointer<Chat> > chatList = chats.values();
        for (int i = 0; i < chatList.size(); i++) {
            chatList[i]->leaveChat();
        }

        chats.clear();
    } catch (const sdc::UserHandlingException &e) {
        success = false;
        message = e.what.c_str();
    }

    valid = false;
    emit q->logoutCompleted(success, message);
}

void SessionPrivate::runDeleteUser(QSharedPointer<const User> user)
{
    Q_Q(Session);
    bool success = true;
    QString message;

    try {
        session->deleteUser(*user->getIceUser().data());
    } catch (const sdc::UserHandlingException &e) {
        success = false;
        message = e.what.c_str();
    }

    if (user->getName() == this->user->getName()) {
        valid = false;
    }

    emit q->deleteUserCompleted(success, message);
}

void SessionPrivate::runRetrieveContactList()
{
    Q_Q(Session);
    bool success = true;
    QString message;
    QStringList contactlistdata;

    try {

        sdc::SecureContainer contactlist = session->retrieveContactList();

        QLOG_TRACE() << __PRETTY_FUNCTION__;
        sdc::ByteSeq decryptedData = user->decrypt(contactlist.data);

        if (!user->verify(decryptedData, contactlist.signature)) {
            success = false;
            message = "Invalid signature";
            goto out;
        }

        QByteArray b1 = sdc::sdcHelper::byteSeqToByteArray(decryptedData);
        contactlistdata = QString(b1).split('\n');

    } catch (const sdc::ContactException &e) {
        success = false;
        message = e.what.c_str();
    } catch (const sdc::SecurityException) {
        success = false;
        message = "Decryption Failed";
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

void SessionPrivate::leaveChatCompletedSlot(bool /* success */,
        const QString /* &message */)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QMutexLocker locker(&chatsMutex);
    chats.remove(qobject_cast<Chat *>(this->sender())->getID());
}

const QSharedPointer<const User> Session::getUser() const
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
    QtConcurrent::run(d, &SessionPrivate::runLogout);
}

void Session::deleteUser(QSharedPointer<const User> user)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runDeleteUser, user);
}

void Session::retrieveUser(const QString &username, const QObject *id)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runRetrieveUser, username, id);
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

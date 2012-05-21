#include "chat.h"
#include "session.h"

#include <QtConcurrentRun>

#include "sdcHelper.h"
#include "SecureDistributedChat.h"
#include "QsLog.h"

namespace sdcc
{

Chat::Chat(sdc::SessionIPrx sessionPrx, Session &session,
           const QString &chatID, const sdc::ByteSeq key)
    : chatID(chatID), session(session), sessionPrx(sessionPrx), key(key)
{
    QMutexLocker locker(&usersMutex);
    QSharedPointer<const User> usr = session.getUser();
    users[usr->getName()] = usr;
}


const QString &Chat::getID() const
{
    return chatID;
}

void Chat::invite(QSharedPointer<const User> user)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QtConcurrent::run(this, &Chat::runInvite, user);
}

void Chat::runInvite(QSharedPointer<const User> user)
{
    bool success = true;
    QString message;

    sdc::User sdcUser = *user->getIceUser().data();

    try {
        sessionPrx->invite(sdcUser, chatID.toStdString(),
                           user->encrypt(key));
    } catch (const sdc::SecurityException &e) {
        success = false;
        message = e.what();
    } catch (const sdc::UserHandlingException &e) {
        success = false;
        message = e.what.c_str();
    } catch (const sdc::InterServerException &e) {
        success = false;
        message = e.what.c_str();
    }

    emit inviteCompleted(success, message);
}

void Chat::send(const QString &msg)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QtConcurrent::run(this, &Chat::runSend, msg);
}

void Chat::runSend(const QString &msg)
{
    bool success = true;
    QString message;

    sdc::Security s;
    try {
        QByteArray unencMsg = msg.toUtf8();
        sdc::ByteSeq encMsg = s.encryptAES(key,
                                           sdc::ByteSeq(unencMsg.begin(), unencMsg.end()));

        sessionPrx->sendMessage(encMsg, chatID.toStdString());

    } catch (const sdc::SecurityException &e) {
        // thrown by encryptAES
        success = false;
        message = e.what();
    } catch (const sdc::MessageException &e) {
        // thrown by sendMessage
        success = false;
        message = e.what.c_str();
    } catch (const sdc::InterServerException &e) {
        // thrown by sendMessage
        success = false;
        message = e.what.c_str();
    }

    emit sendCompleted(success, message);
}

void Chat::receiveMessage(QSharedPointer<const User> participant, const sdc::ByteSeq &encMsg)
{
    QMutexLocker locker(&usersMutex);
    if (!users.contains(participant->getName())) {
        QLOG_ERROR() << QString("Received message for chat '%1' from user '%2', "
                                "who is not in the chat").arg(chatID)
                     .arg(participant->getName());
        return;
    }
    locker.unlock();

    try {
        sdc::Security s;
        sdc::ByteSeq decMsg = s.decryptAES(key, encMsg);
        QString msg = QString::fromStdString(sdc::sdcHelper::getBinaryString(decMsg));

        QLOG_TRACE() << "Received message:" << msg;

        // TODO: What if we receive invalid UTF8 code points? QString doesn't
        // give us any information on this. This should only occur if other clients
        // encode their messages differently though.
        emit messageReceived(participant, msg);

    } catch (const sdc::SecurityException &e) {
        // TODO: Workaround cause I don't know how to report generic errors...
        emit messageReceived(participant, "<received message but could not decrypt it>");
    }

}

void Chat::leaveChat()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QtConcurrent::run(this, &Chat::runLeaveChat);
}

void Chat::runLeaveChat()
{
    bool success = true;
    QString message;

    try {
        sessionPrx->leaveChat(chatID.toStdString());

    } catch (const sdc::SessionException &e) {
        success = false;
        message = e.what.c_str();
    } catch (const sdc::InterServerException &e) {
        success = false;
        message = e.what.c_str();
    }

    emit leaveChatCompleted(success, message);
}

void Chat::addChatParticipant(QSharedPointer<const User> participant)
{
    QMutexLocker locker(&usersMutex);
    users[participant->getName()] = participant;
    emit userJoined(participant);
}

void Chat::removeChatParticipant(QSharedPointer<const User> participant)
{
    QMutexLocker locker(&usersMutex);
    if (!users.contains(participant->getName())) {
        QLOG_ERROR() << QString("Remove non-participant user '%1' from chat '%2'")
                     .arg(participant->getName()).arg(chatID);
        return;
    }

    users.remove(participant->getName());
    emit userLeft(participant);
}

QList<QSharedPointer<const User> > Chat::getUserList()
{
    QMutexLocker locker(&usersMutex);
    return users.values();
}

}

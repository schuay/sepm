#include "chat.h"
#include "session.h"

#include <QtConcurrentRun>

#include <Security.h>
#include "SecureDistributedChat.h"

namespace sdcc
{
Chat::Chat(sdc::SessionIPrx sessionPrx, Session &session,
           const QString &chatID) throw (sdc::SecurityException)
    : chatID(chatID), session(session), sessionPrx(sessionPrx)
{
    sdc::Security s;
    key = s.genAESKey(SESSION_KEY_SIZE);

    users.append(session.getUser());
}

void Chat::invite(const User &user)
{
    QtConcurrent::run(this, &Chat::runInvite, user);
}

void Chat::runInvite(const User &user)
{
    bool success = true;
    QString message;

    sdc::User sdcUser = *user.getIceUser().data();
    sdc::Security s;

    try {
        sessionPrx->invite(sdcUser, chatID.toStdString(),
                           s.encryptRSA(sdcUser.publicKey, key));
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

void Chat::receiveMessage(const User &participant, const sdc::ByteSeq &encMsg)
{
    try {
        // kinda complicated, is there not a standard method somewhere??
        // convert vector<byte> --> byte* --> QString
        sdc::Security s;
        sdc::ByteSeq decMsg = s.decryptAES(key, encMsg);
        char *umsg = new char[decMsg.size()];
        char *j = umsg;

        for (sdc::ByteSeq::iterator i = decMsg.begin(); i != decMsg.end(); ++i) {
            *(j++) = *i;
        }

        QString msg = QString::fromUtf8(umsg, decMsg.size());
        delete umsg;

        // TODO: What if we receive invalid UTF8 code points? QString doesn't
        // give us any information on this. This should only occur if other clients
        // encode their messages differently though.
        emit messageReceived(participant, msg);

    } catch (sdc::SecurityException e) {
        // TODO: Workaround cause I don't know how to report generic errors...
        emit messageReceived(participant, "<received message but could not decrypt it>");
    }

}

}

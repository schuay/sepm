#include "chat.h"
#include "session.h"

#include <QtConcurrentRun>

#include <Security.h>
#include "SecureDistributedChat.h"

namespace sdcc
{
Chat::Chat(const Session &session, const QString &chatID)
throw ( sdc::SecurityException )
    : chatID(chatID), session(session)
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
        session.getSDCSession()->invite(sdcUser, chatID.toStdString(),
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
}

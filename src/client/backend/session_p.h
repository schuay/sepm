#ifndef _SESSION_P_H
#define _SESSION_P_H

#include "session.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QMutexLocker>

#include "QsLog.h"

#include "Security.h"

namespace sdcc
{

class SessionPrivate : public QObject, public sdc::ChatClientCallbackI
{
    Q_OBJECT

    SessionPrivate(Session *q, Ice::CommunicatorPtr c)
        : communicator(c), q_ptr(q) { }

    void initChat(const sdc::StringSeq &cUsers, const std::string &chatID,
                  const sdc::ByteSeq &sessionKeyEnc, const Ice::Current &);

    void addChatParticipant(const sdc::User &participant,
                            const std::string &chatID, const Ice::Current &);

    void removeChatParticipant(const sdc::User &/*participant*/,
                               const std::string &/*chatID*/, const Ice::Current &);

    void appendMessageToChat(const sdc::ByteSeq &message, const std::string &chatID,
                             const sdc::User &participant, const Ice::Current &);

    std::string echo(const std::string &message, const Ice::Current &);

    void runRetrieveUser(const QString &username, const QObject *id);

    void runInitChat();

    void runLogout();

    void runDeleteUser(QSharedPointer<const User> user);

    ~SessionPrivate();

    QMutex chatsMutex;
    QMutex usersMutex;
    QMap<QString, QSharedPointer<Chat> > chats;
    QMap<QString, QSharedPointer<const User> > users;

    Ice::CommunicatorPtr communicator;
    sdc::SessionIPrx session;

    QSharedPointer<const LoginUser> user;

private:
    Session *q_ptr;
    Q_DECLARE_PUBLIC(Session)

    QSharedPointer<const User> retrieveUser(const QString &username);
    QSharedPointer<const User> getUser(const QString &username);

private slots:
    void leaveChatCompletedSlot(bool /* success */, const QString /* &message */);
};

}

#endif /* _SESSION_P_H */

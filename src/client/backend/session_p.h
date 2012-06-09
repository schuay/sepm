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
                  const sdc::ByteSeq &sessionKeyEnc, const Ice::Current &)
    throw(sdc::ChatClientCallbackException);

    void addChatParticipant(const sdc::User &participant,
                            const std::string &chatID, const Ice::Current &)
    throw(sdc::ParticipationException);

    void removeChatParticipant(const sdc::User &participant,
                               const std::string &chatID, const Ice::Current &)
    throw(sdc::ParticipationException);

    void appendMessageToChat(const sdc::ByteSeq &message, const std::string &chatID,
                             const sdc::User &participant, const Ice::Current &)
    throw(sdc::MessageCallbackException);

    std::string echo(const std::string &message, const Ice::Current &)
    throw(sdc::SDCException);

    void runRetrieveUser(const QString &username, const QObject *id);

    void runInitChat();

    void runLogout();

    void runDeleteUser(QSharedPointer<const User> user);

    void runRetrieveContactList();

    void runRetrieveLoglist();

    void runRetrieveLog(const QDateTime &time, const QString &chat);

    void runSaveContactList(const QStringList &contactlist);

    ~SessionPrivate();

    QMutex chatsMutex;
    QMutex usersMutex;
    QMap<QString, QSharedPointer<Chat> > chats;
    QMap<QString, QSharedPointer<const User> > users;

    Ice::CommunicatorPtr communicator;
    sdc::SessionIPrx session;

    bool valid;

    QSharedPointer<const LoginUser> user;

private:
    Session *q_ptr;
    Q_DECLARE_PUBLIC(Session)

    QSharedPointer<const User> retrieveUser(const QString &username);
    QSharedPointer<const User> getUser(const QString &username);
    void leaveAllChats(void);

private slots:
    void leaveChatCompletedSlot();
};

}

#endif /* _SESSION_P_H */

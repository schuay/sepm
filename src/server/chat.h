#ifndef CHAT_H
#define CHAT_H

#include <QMap>
#include <QMutex>
#include <QSharedPointer>

#include "SecureDistributedChat.h"
#include "participant.h"

namespace sdcs
{

class Chat
{
public:
    Chat(const QString &chatID);
    virtual ~Chat();

    /**
     * Appends message from user to all participants.
     */
    virtual void appendMessageFrom(const sdc::User &user, const sdc::ByteSeq &message) = 0;

    /**
     * Invites user to the chat with the given session key.
     */
    virtual void inviteUser(const sdc::User &user, const sdc::ByteSeq &sessionKey) = 0;

    /**
     * Removes user from the chat and notifies all remaining participants.
     */
    virtual void leaveChat(const QString &user) = 0;

    QString getChatID() const;

protected:
    const QString chatID;

    QMap<QString, QSharedPointer<Participant> > participants;
    QMutex participantsMutex;
};

/* owned by server, not by session. pointer kept in global list. destroyed only when empty. */
class LocalChat : public Chat
{
public:
    LocalChat(const QString &chatID);

    void appendMessageFrom(const sdc::User &user, const sdc::ByteSeq &message);
    void inviteUser(const sdc::User &user, const sdc::ByteSeq &message);
    void leaveChat(const QString &user);
};

/* owned by session, pointer kept in session. destroyed on leave or logout. */
class RemoteChat : public Chat
{
public:
    RemoteChat(const QString &chatID);

    void appendMessageFrom(const sdc::User &user, const sdc::ByteSeq &message);
    void inviteUser(const sdc::User &user, const sdc::ByteSeq &message);
    void leaveChat(const QString &user);

private:
    sdc::InterServerIPrx interServer; /* one per remote server */
};

}

#endif

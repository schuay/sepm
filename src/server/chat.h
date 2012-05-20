#ifndef CHAT_H
#define CHAT_H

#include "SecureDistributedChat.h"

#include "participant.h"

namespace sdcs
{

class Chat
{
public:
    /* appends message to all participant client callbacks. */
    virtual void appendMessageFrom(sdc::User, sdc::ByteSeq) = 0;
    virtual void inviteUser(const Participant &, sdc::ByteSeq) = 0;
    virtual void leaveChat(const Participant &) = 0;
    virtual ~Chat() = 0;

private:
    QString chatID;
    QList<Participant> participants;
};

/* owned by server, not by session. pointer kept in global list. destroyed only when empty. */
class LocalChat : public Chat
{
public:
    void appendMessageFrom(sdc::User, sdc::ByteSeq);
    void inviteUser(const Participant &, sdc::ByteSeq);
    void leaveChat(const Participant &);
};

/* owned by session, pointer kept in session. destroyed on leave or logout. */
class RemoteChat : public Chat
{
public:
    void appendMessageFrom(sdc::User, sdc::ByteSeq);
    void inviteUser(const Participant &, sdc::ByteSeq);
    void leaveChat(const Participant &);

private:
    sdc::InterServerIPrx interServer; /* one per remote server */
};

}

#endif

#include "SecureDistributedChat.h"

class Chat
{
public:
    /* appends message to all participant client callbacks. */
    virtual void appendMessageFrom(User from, ByteSeq message);
    virtual void inviteUser(Participant user, ByteSeq sessionKey);
    virtual void leaveChat(Participant user);

private:
    QString chatID;
    QList<Participant> participants;
};

/* owned by server, not by session. pointer kept in global list. destroyed only when empty. */
class LocalChat : public Chat
{
};

/* owned by session, pointer kept in session. destroyed on leave or logout. */
class RemoteChat : public Chat
{
private:
    sdc::InterServerIPrx interServer; /* one per remote server */
};

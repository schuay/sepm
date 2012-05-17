#include "SecureDistributedChat.h"

#include "participant.h"

using namespace sdc;

namespace sdcs
{

class Chat
{
public:
    /* appends message to all participant client callbacks. */
    virtual void appendMessageFrom(User, ByteSeq) {}
    virtual void inviteUser(const Participant &, ByteSeq) {}
    virtual void leaveChat(const Participant &) {}
    virtual ~Chat() {}

private:
    QString chatID;
    QList<Participant> participants;
};

/* owned by server, not by session. pointer kept in global list. destroyed only when empty. */
class LocalChat : public Chat
{
public:
    void appendMessageFrom(User, ByteSeq) {}
    void inviteUser(const Participant &, ByteSeq) {}
    void leaveChat(const Participant &) {}
};

/* owned by session, pointer kept in session. destroyed on leave or logout. */
class RemoteChat : public Chat
{
public:
    void appendMessageFrom(User, ByteSeq) {}
    void inviteUser(const Participant &, ByteSeq) {}
    void leaveChat(const Participant &) {}

private:
    sdc::InterServerIPrx interServer; /* one per remote server */
};

}

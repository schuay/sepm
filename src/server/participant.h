#include <QStringList>

#include "SecureDistributedChat.h"

/* only exists within a chat */
class Participant
{
public:
    virtual void invite(QStringList users, sdc::ByteSeq sessionKey) = 0;
    virtual void addChatParticipant(const sdc::User &participant) = 0;
    virtual void removeChatParticipant(const sdc::User &participant) = 0;
    virtual void appendMessageToChat(const sdc::User &user, const QString &message) = 0;

protected:
    sdc::User user;
    QString chatID;
};

class LocalParticipant : public Participant
{
public:
    LocalParticipant(sdc::User sdcUser, const QString &chatID);

private:
    void echo(const QString &message);
    sdc::ChatClientCallbackIPrx proxy;
};

class RemoteParticipant : public Participant
{
public:
    RemoteParticipant(sdc::User sdcUser, const QString &chatID);

private:
    sdc::InterServerIPrx proxy;
};

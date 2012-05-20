#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <QStringList>

#include "SecureDistributedChat.h"

namespace sdcs
{

/* only exists within a chat */
class Participant
{
public:
    virtual void invite(QStringList users, sdc::ByteSeq sessionKey) = 0;
    virtual void addChatParticipant(const sdc::User &participant) = 0;
    virtual void removeChatParticipant(const sdc::User &participant) = 0;
    virtual void appendMessageToChat(const sdc::User &user, const QString &message) = 0;
    virtual ~Participant() = 0;

protected:
    sdc::User user;
    QString chatID;
};

class LocalParticipant : public Participant
{
public:
    LocalParticipant(sdc::User sdcUser, const QString &chatID);
    virtual void invite(QStringList , sdc::ByteSeq);
    virtual void addChatParticipant(const sdc::User &);
    virtual void removeChatParticipant(const sdc::User &);
    virtual void appendMessageToChat(const sdc::User &, const QString &);

private:
    void echo(const QString &message);
    sdc::ChatClientCallbackIPrx proxy;
};

class RemoteParticipant : public Participant
{
public:
    RemoteParticipant(sdc::User sdcUser, const QString &chatID);
    virtual void invite(QStringList , sdc::ByteSeq);
    virtual void addChatParticipant(const sdc::User &);
    virtual void removeChatParticipant(const sdc::User &);
    virtual void appendMessageToChat(const sdc::User &, const QString &);

private:
    sdc::InterServerIPrx proxy;
};

}

#endif

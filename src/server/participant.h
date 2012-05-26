#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <QStringList>

#include "SecureDistributedChat.h"

namespace sdcs
{

/**
 * Represents a user participating in a chat.
 */
class Participant
{
public:
    Participant(const sdc::User &user, const QString &chatID);
    virtual ~Participant();

    virtual void invite(QStringList users, sdc::ByteSeq sessionKey) = 0;
    virtual void addChatParticipant(const sdc::User &participant) = 0;
    virtual void removeChatParticipant(const sdc::User &participant) = 0;
    virtual void appendMessageToChat(const sdc::User &user, const QString &message) = 0;

protected:
    const sdc::User self;
    const QString chatID;
};

class LocalParticipant : public Participant
{
public:
    LocalParticipant(const sdc::User &user, const QString &chatID);

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
    RemoteParticipant(const sdc::User &user, const QString &chatID);

    virtual void invite(QStringList , sdc::ByteSeq);
    virtual void addChatParticipant(const sdc::User &);
    virtual void removeChatParticipant(const sdc::User &);
    virtual void appendMessageToChat(const sdc::User &, const QString &);

private:
    sdc::InterServerIPrx proxy;
};

}

#endif

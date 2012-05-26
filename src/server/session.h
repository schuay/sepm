#ifndef SESSION_H
#define SESSION_H

#include <Ice/Ice.h>
#include <QMap>
#include <QSharedPointer>
#include <QMutex>

#include "SecureDistributedChat.h"
#include "chat.h"

namespace sdcs
{

class Server;

class Session : public sdc::SessionI
{
public:
    Session(const sdc::User &self, sdc::ChatClientCallbackIPrx callback);

    /**
     * Logout the user this session belongs to.
     */
    void logout(const Ice::Current &) throw(sdc::UserHandlingException);

    /**
     * Retrieve the User object of the user with ID userID.
     *
     * If the requested user is not at the clients home server,
     * the request is forwarded to the home server
     * of that client.
     * The returned User object contains the key necessary for
     * sending that user encrypted content.
     */
    sdc::User retrieveUser(const std::string &userID, const Ice::Current &)
    throw(sdc::UserHandlingException, sdc::InterServerException);

    /** Initialize a new chat and return the according chat ID.
     *  The client will very likely want to generate a session key
     *  for the new chat when calling initChat(). See invite().
     */
    std::string initChat(const Ice::Current &) throw(sdc::SessionException);

    /**
     * Leave the chat with ID chatID.
     *
     * If the chat is not at the clients home server,
     * the request is forwarded to the home server of the chat.
     * The receiving server will want to forward the news
     * to all participants of that chat.
     */
    void leaveChat(const std::string &chatID, const Ice::Current &)
    throw(sdc::SessionException, sdc::InterServerException);

    /**
     * Invite the user participant to the chat chatID.
     * All communication in that chat is encrypted using
     * sessionKey.
     *
     * If the user being invited is not at the clients
     * home server, the invite is forwarded to the home server
     * of that respective client.
     */
    void invite(const sdc::User &participant, const std::string &chatID,
                const sdc::ByteSeq &sessionKey, const Ice::Current &)
    throw(sdc::UserHandlingException, sdc::InterServerException);

    /**
     * Send a message to a chat.
     *
     * If the message is sent to a chat that is not managed
     * by the clients home server, the message is forwarded.
     */
    void sendMessage(const sdc::ByteSeq &message, const std::string &chatID, const Ice::Current &)
    throw(sdc::MessageException, sdc::InterServerException);

    /**
     * Delete a user. This will most likely be restricted to
     * the use of clients with administrative privileges.
     */
    void deleteUser(const sdc::User &participant, const Ice::Current &)
    throw(sdc::UserHandlingException);

    /**
     * Save a log of a chat. The log is encrypted and signed,
     * see the documentation of SecureContainer for
     * more information.
     * Keep in mind that the user the log belongs to
     * is identified by the session itself.
     */
    void saveLog(const std::string &chatID, Ice::Long timestamp,
                 const sdc::SecureContainer &log, const Ice::Current &)
    throw(sdc::LogException);

    /**
     * Retrieve all logs of the user the session belongs to.
     */
    sdc::Loglist retrieveLoglist(const Ice::Current &) throw(sdc::LogException);

    /**
     * Retrieve a signed and encrypted log of a chat.
     */
    sdc::SecureContainer retrieveLog(const std::string &chatID, Ice::Long timestamp,
                                     const Ice::Current &)
    throw(sdc::LogException);

    /**
     * Save the contact list of a user. The contact list is
     * encrypted and signed. See the documentation of
     * SecureContainer for more information.
     * Keep in mind that the user the contact list belongs to
     * is identified by the session itself.
     */
    void saveContactList(const sdc::SecureContainer &contactList, const Ice::Current &)
    throw(sdc::ContactException);

    /**
     * Retrieve the signed and encrypted contact list of a user.
     */
    sdc::SecureContainer retrieveContactList(const Ice::Current &)
    throw(sdc::ContactException);

    /**
     * Add chat to the list of active chats and return the client callback.
     */
    sdc::ChatClientCallbackIPrx addChat(QSharedPointer<Chat> chat);

private:
    const sdc::User self;
    const sdc::ChatClientCallbackIPrx callback;

    /**
     * Stores a list of all active chats the session is participating in by chat ID.
     */
    QMap<QString, QSharedPointer<Chat> > chats;
    QMutex chatsMutex;
};

}

#endif

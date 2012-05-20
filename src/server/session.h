#ifndef SESSION_H
#define SESSION_H

#include "SecureDistributedChat.h"
#include <Ice/Ice.h>

namespace sdcs
{

class Session : public sdc::SessionI
{
public:
    Session(const sdc::User &user, sdc::ChatClientCallbackIPrx callback);

    /* destroy self, notify all open chats */
    void logout(const Ice::Current &) throw(sdc::UserHandlingException);

    /* local user: db lookup
     * remote: create interserver, forward call, return. */
    sdc::User retrieveUser(const std::string &userID, const Ice::Current &)
    throw(sdc::UserHandlingException, sdc::InterServerException);

    /* gen ID (protected by sema, uuid@serverurl)
     * create LocalChat, insert in global and own list, add self as participant.
     * idea: server attaches to chatCreated signal.
     * */
    std::string initChat(const Ice::Current &) throw(sdc::SessionException);

    /* notify chat, remove from local list */
    void leaveChat(const std::string &chatID, const Ice::Current &)
    throw(sdc::SessionException, sdc::InterServerException);

    /* check consistency (chat exists, chat joined)
     * create participant (remote or local), call invite. */
    void invite(const sdc::User &participant, const std::string &chatID,
                const sdc::ByteSeq &sessionKey, const Ice::Current &)
    throw(sdc::UserHandlingException, sdc::InterServerException);

    /* looks up chat and calls appendMessageFrom. */
    void sendMessage(const sdc::ByteSeq &message, const std::string &chatID, const Ice::Current &)
    throw(sdc::MessageException, sdc::InterServerException);

    /* check auth, do db ops, logout (make sure everything is consistent), destroy session */
    void deleteUser(const sdc::User &participant, const Ice::Current &)
    throw(sdc::UserHandlingException);


    /* --------------------- */

    void saveLog(const std::string &chatID, long timestamp,
                 const sdc::SecureContainer &log, const Ice::Current &)
    throw(sdc::LogException);

    sdc::Loglist retrieveLoglist(const Ice::Current &) throw(sdc::LogException);

    sdc::SecureContainer retrieveLog(const std::string &chatID, long timestamp,
                                     const Ice::Current &)
    throw(sdc::LogException);

    void saveContactList(const sdc::SecureContainer &contactList, const Ice::Current &)
    throw(sdc::ContactException);

    sdc::SecureContainer retrieveContactList(const Ice::Current &)
    throw(sdc::ContactException);

    /*
    signals:
      chatCreated(Chatptr, ...);
      chatDestroyed(Chatptr, ...);
    */

private:
    const sdc::User user;
    const sdc::ChatClientCallbackIPrx callback;
//    QMap<QString, Chat> chats;
//    LocalParticipant self;
};

}

#endif

#include "SecureDistributedChat.h"
#include <Ice/Ice.h>

using namespace sdc;

namespace sdcs
{

class Session : public SessionI
{

signals:
    /* pseudocode?
      chatCreated(Chatptr, ...);
      chatDestroyed(Chatptr, ...);
    */

public:
    /* destroy self, notify all open chats */
    void logout(const Ice::Current&) // throw UserHandlingException;

    /* local user: db lookup
     * remote: create interserver, forward call, return. */
    User retrieveUser(const std::string &userID, const Ice::Current&); // throw UserHandlingException, InterServerException;

    /* gen ID (protected by sema, uuid@serverurl)
     * create LocalChat, insert in global and own list, add self as participant.
     * idea: server attaches to chatCreated signal.
     * */
    std::string initChat(const Ice::Current&); // throw SessionException;

    /* notify chat, remove from local list */
    void leaveChat(const std::string &chatID, const Ice::Current&); // throw SessionException, InterServerException;

    /* check consistency (chat exists, chat joined)
     * create participant (remote or local), call invite. */
    void invite(const User &participant, const std::string &chatID,
                const ByteSeq &sessionKey, const Ice::Current&); // throw UserHandlingException, InterServerException;

    /* looks up chat and calls appendMessageFrom. */
    void sendMessage(const ByteSeq &message, const std::string &chatID, const Ice::Current&); // throw MessageException, InterServerException;

    /* check auth, do db ops, logout (make sure everything is consistent), destroy session */
    void deleteUser(const User &participant, const Ice::Current&); //throw UserHandlingException;


    /* --------------------- */

    void saveLog(const std::string &chatID, long timestamp, const SecureContainer &log, const Ice::Current&); // throw LogException;
    Loglist retrieveLoglist(const Ice::Current &); //throw LogException;
    SecureContainer retrieveLog(const std::string &chatID, long timestamp, const Ice::Current&); // throw LogException;
    void saveContactList(const SecureContainer &contactList, const Ice::Current&); // throw ContactException;
    SecureContainer retrieveContactList(const Ice::Current&);//  throw ContactException;

private:
    QMap<QString, Chat> chats;
    LocalParticipant self;
};

}

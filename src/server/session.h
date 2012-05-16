#include "SecureDistributedChat.h"

using namespace sdc;

namespace sdcs
{

class Session : public SessionI
{
signals:
    chatCreated(Chatptr, ...);
    chatDestroyed(Chatptr, ...);

public:
    /* destroy self, notify all open chats */
    void logout() throw UserHandlingException;

    /* local user: db lookup
     * remote: create interserver, forward call, return. */
    User retrieveUser(std::string userID) throw UserHandlingException, InterServerException;

    /* gen ID (protected by sema, uuid@serverurl)
     * create LocalChat, insert in global and own list, add self as participant.
     * idea: server attaches to chatCreated signal.
     * */
    std::string initChat() throw SessionException;

    /* notify chat, remove from local list */
    void leaveChat(std::string chatID) throw SessionException, InterServerException;

    /* check consistency (chat exists, chat joined)
     * create participant (remote or local), call invite. */
    void invite(User participant, std::string chatID,
                ByteSeq sessionKey) throw UserHandlingException, InterServerException;

    /* looks up chat and calls appendMessageFrom. */
    void sendMessage(ByteSeq message, std::string chatID) throw MessageException, InterServerException;

    /* check auth, do db ops, logout (make sure everything is consistent), destroy session */
    void deleteUser(User participant) throw UserHandlingException;


    /* --------------------- */

    void saveLog(std::string chatID, long timestamp, SecureContainer log) throw LogException;
    Loglist retrieveLoglist() throw LogException;
    SecureContainer retrieveLog(std::string chatID, long timestamp) throw LogException;
    void saveContactList(SecureContainer contactList) throw ContactException;
    SecureContainer retrieveContactList() throw ContactException;

private:
    QMap<QString, Chat> chats;
    Participant self;
};

}

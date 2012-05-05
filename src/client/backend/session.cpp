#include "session.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "QsLog.h"

namespace sdcc
{

class ChatClientCallback : public sdc::ChatClientCallbackI
{
public:
    void initChat(const sdc::StringSeq& /*users*/, const std::string& /*chatID*/,
                  const sdc::ByteSeq& /*key*/, const Ice::Current &) {
        QLOG_TRACE() << "ChatClientCallback::initChat()";
    }

    void addChatParticipant(const sdc::User& /*participant*/,
                            const ::std::string& /*chatID*/, const Ice::Current &) {
        QLOG_TRACE() << "ChatClientCallback::addChatParticipant()";
    }

    void removeChatParticipant(const sdc::User& /*participant*/,
                               const ::std::string& /*chatID*/, const Ice::Current &) {
        QLOG_TRACE() << "ChatClientCallback::removeChatParticipant()";
    }

    void appendMessageToChat(const sdc::ByteSeq& /*message*/, const ::std::string& /*chatID*/,
                             const sdc::User& /*participant*/, const Ice::Current &) {
        QLOG_TRACE() << "ChatClientCallback::appendMessageToChat()";
    }

    std::string echo(const ::std::string& message, const Ice::Current &) {
        QLOG_TRACE() << "ChatClientCallback::echo()";
        return message;
    }
};

Session::Session(const User &user, const QString &pwd, sdc::AuthenticationIPrx auth)
    : clientCallback(new ChatClientCallback), authenticationProxy(auth)
{
    sdc::User sdcUser = *user.getIceUser().data();

    Ice::CommunicatorPtr communicator = auth->ice_getCommunicator();
    Ice::ObjectAdapterPtr adapter = communicator
                                    ->createObjectAdapterWithEndpoints("ChatClientCallback", "default");
    Ice::Identity identity = { IceUtil::generateUUID(), "" };

    sdc::ChatClientCallbackIPtr callback(clientCallback.data());
    adapter->add(callback, identity);
    adapter->activate();
    auth->ice_getConnection()->setAdapter(adapter);

    session = auth->login(sdcUser, pwd.toStdString(), identity);

    if (!session)
        throw sdc::SDCException("Login failed");
}

}

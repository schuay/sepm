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
        QLOG_TRACE() << __PRETTY_FUNCTION__;
    }

    void addChatParticipant(const sdc::User& /*participant*/,
                            const ::std::string& /*chatID*/, const Ice::Current &) {
        QLOG_TRACE() << __PRETTY_FUNCTION__;
    }

    void removeChatParticipant(const sdc::User& /*participant*/,
                               const ::std::string& /*chatID*/, const Ice::Current &) {
        QLOG_TRACE() << __PRETTY_FUNCTION__;
    }

    void appendMessageToChat(const sdc::ByteSeq& /*message*/, const ::std::string& /*chatID*/,
                             const sdc::User& /*participant*/, const Ice::Current &) {
        QLOG_TRACE() << __PRETTY_FUNCTION__;
    }

    std::string echo(const ::std::string& message, const Ice::Current &) {
        QLOG_TRACE() << __PRETTY_FUNCTION__;
        return message;
    }
};

struct SessionPrivate {
    SessionPrivate(sdc::AuthenticationIPrx auth)
        : clientCallback(new ChatClientCallback), authenticationProxy(auth) { }

    QList<QSharedPointer<User> > users;
    QSharedPointer<ChatClientCallback> clientCallback;
    sdc::AuthenticationIPrx authenticationProxy;
    sdc::SessionIPrx session;
    QList<QSharedPointer<Chat> > chats;
};

Session::Session(const User &user, const QString &pwd, sdc::AuthenticationIPrx auth)
    : d_ptr(new SessionPrivate(auth))
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);

    sdc::User sdcUser = *user.getIceUser().data();

    Ice::CommunicatorPtr communicator = auth->ice_getCommunicator();
    Ice::ConnectionPtr connection = auth->ice_getConnection();
    Ice::ObjectAdapterPtr adapter = connection->getAdapter();
    Ice::Identity identity = { IceUtil::generateUUID(), "" };

    if (!adapter) {
        adapter = communicator->createObjectAdapterWithEndpoints("ChatClientCallback", "default");
    }

    sdc::ChatClientCallbackIPtr callback(d->clientCallback.data());
    adapter->add(callback, identity);
    adapter->activate();
    auth->ice_getConnection()->setAdapter(adapter);

    d->session = auth->login(sdcUser, pwd.toStdString(), identity);

    if (!d->session)
        throw sdc::SDCException("Login failed");
}

}

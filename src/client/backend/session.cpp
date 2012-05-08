#include "session.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QtConcurrentRun>

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
    SessionPrivate(Session *q, Ice::CommunicatorPtr c)
        : clientCallback(new ChatClientCallback), communicator(c), q_ptr(q) { }

    void runInitChat() {
        Q_Q(Session);
        bool success = true;
        QString message;
        Chat *c = NULL;

        try {
            c = new Chat(*q, session->initChat().c_str());
        } catch (const sdc::SessionException &e) {
            success = false;
            message = e.what.c_str();
        } catch (const sdc::SecurityException &e) {
            success = false;
            message = e.what();
        }

        QSharedPointer<Chat> cp(c);
        chats.append(cp);

        emit q->initChatCompleted(cp, success, message);
    }

    void runLogout() {
        Q_Q(Session);
        bool success = true;
        QString message;

        try {
            session->logout();
        } catch (const sdc::UserHandlingException &e) {
            success = false;
            message = e.what.c_str();
        }

        emit q->logoutCompleted(success, message);
    }

    void runDeleteUser(const User &user) {
        Q_Q(Session);
        bool success = true;
        QString message;

        try {
            session->deleteUser(*user.getIceUser().data());
        } catch (const sdc::UserHandlingException &e) {
            success = false;
            message = e.what.c_str();
        }

        emit q->deleteUserCompleted(success, message);
    }

    ~SessionPrivate() {
        if (communicator) {
            communicator->destroy();
        }
    }

    QList<QSharedPointer<User> > users;
    QList<QSharedPointer<Chat> > chats;

    /* Note that all Ice objects are destroyed automatically
     * when communicator->destroy() is called. This includes
     * the clientCallback, which is why we're not wrapping it in
     * a smart pointer. */
    ChatClientCallback *clientCallback;
    Ice::CommunicatorPtr communicator;
    sdc::SessionIPrx session;

    QSharedPointer<User> user;

private:
    Session *q_ptr;
    Q_DECLARE_PUBLIC(Session)
};

sdc::SessionIPrx Session::getSDCSession() const
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(const Session);

    return d->session;
}

const QSharedPointer<User> Session::getUser() const
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(const Session);

    return d->user;
}

Session::Session(const User &user, const QString &pwd, sdc::AuthenticationIPrx auth)
    : d_ptr(new SessionPrivate(this, auth->ice_getCommunicator()))
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);

    sdc::User sdcUser = *user.getIceUser().data();
    d->user = QSharedPointer<User> (new User(sdcUser));

    Ice::ConnectionPtr connection = auth->ice_getConnection();
    Ice::ObjectAdapterPtr adapter = connection->getAdapter();
    Ice::Identity identity = { IceUtil::generateUUID(), "" };

    if (!adapter) {
        adapter = d->communicator->createObjectAdapterWithEndpoints("ChatClientCallback", "default");
    }

    sdc::ChatClientCallbackIPtr callback(d->clientCallback);
    adapter->add(callback, identity);
    adapter->activate();
    auth->ice_getConnection()->setAdapter(adapter);

    d->session = auth->login(sdcUser, pwd.toStdString(), identity);

    if (!d->session)
        throw sdc::SDCException("Login failed");
}

void Session::initChat()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runInitChat);
}

void Session::logout()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runLogout);
}

void Session::deleteUser(const User &user)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    Q_D(Session);
    QtConcurrent::run(d, &SessionPrivate::runDeleteUser, user);
}

Session::~Session()
{
    Q_D(Session);
    delete d;
}

}

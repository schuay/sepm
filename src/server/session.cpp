#include "session.h"

#include "QsLog.h"

namespace sdcs
{

Session::Session(const sdc::User &user, sdc::ChatClientCallbackIPrx callback)
    : user(user), callback(callback)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void Session::logout(const Ice::Current &) throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

sdc::User Session::retrieveUser(const std::string &/*userID*/, const Ice::Current &)
throw(sdc::UserHandlingException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    sdc::User u;
    return u;
}

std::string Session::initChat(const Ice::Current &) throw(sdc::SessionException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    return "";
}

void Session::leaveChat(const std::string &/*chatID*/, const Ice::Current &)
throw(sdc::SessionException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void Session::invite(const sdc::User &/*participant*/, const std::string &/*chatID*/,
                     const sdc::ByteSeq &/*sessionKey*/, const Ice::Current &)
throw(sdc::UserHandlingException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void Session::sendMessage(const sdc::ByteSeq &/*message*/, const std::string &/*chatID*/, const Ice::Current &)
throw(sdc::MessageException, sdc::InterServerException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void Session::deleteUser(const sdc::User &/*participant*/, const Ice::Current &)
throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

void Session::saveLog(const std::string &/*chatID*/, Ice::Long /*timestamp*/,
                      const sdc::SecureContainer &/*log*/, const Ice::Current &)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

sdc::Loglist Session::retrieveLoglist(const Ice::Current &) throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    sdc::Loglist l;
    return l;
}

sdc::SecureContainer Session::retrieveLog(const std::string &/*chatID*/,
        Ice::Long /*timestamp*/, const Ice::Current &)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    sdc::SecureContainer c;
    return c;
}

void Session::saveContactList(const sdc::SecureContainer &/*contactList*/, const Ice::Current &)
throw(sdc::ContactException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
}

sdc::SecureContainer Session::retrieveContactList(const Ice::Current &)
throw(sdc::ContactException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    sdc::SecureContainer c;
    return c;
}

}

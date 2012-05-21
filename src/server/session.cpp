#include "session.h"

namespace sdcs
{

Session::Session(const sdc::User &user, sdc::ChatClientCallbackIPrx callback)
    : user(user), callback(callback)
{
}

void Session::logout(const Ice::Current &) throw(sdc::UserHandlingException) { }

sdc::User Session::retrieveUser(const std::string &/*userID*/, const Ice::Current &)
throw(sdc::UserHandlingException, sdc::InterServerException)
{
    sdc::User u;
    return u;
}

std::string Session::initChat(const Ice::Current &) throw(sdc::SessionException)
{
    return "";
}

void Session::leaveChat(const std::string &/*chatID*/, const Ice::Current &)
throw(sdc::SessionException, sdc::InterServerException) { }

void Session::invite(const sdc::User &/*participant*/, const std::string &/*chatID*/,
                     const sdc::ByteSeq &/*sessionKey*/, const Ice::Current &)
throw(sdc::UserHandlingException, sdc::InterServerException) { }

void Session::sendMessage(const sdc::ByteSeq &/*message*/, const std::string &/*chatID*/, const Ice::Current &)
throw(sdc::MessageException, sdc::InterServerException) { }

void Session::deleteUser(const sdc::User &/*participant*/, const Ice::Current &)
throw(sdc::UserHandlingException) { }

void Session::saveLog(const std::string &/*chatID*/, Ice::Long /*timestamp*/,
                      const sdc::SecureContainer &/*log*/, const Ice::Current &)
throw(sdc::LogException) { }

sdc::Loglist Session::retrieveLoglist(const Ice::Current &) throw(sdc::LogException)
{
    sdc::Loglist l;
    return l;
}

sdc::SecureContainer Session::retrieveLog(const std::string &/*chatID*/,
        Ice::Long /*timestamp*/, const Ice::Current &)
throw(sdc::LogException)
{
    sdc::SecureContainer c;
    return c;
}

void Session::saveContactList(const sdc::SecureContainer &/*contactList*/, const Ice::Current &)
throw(sdc::ContactException) { }

sdc::SecureContainer Session::retrieveContactList(const Ice::Current &)
throw(sdc::ContactException)
{
    sdc::SecureContainer c;
    return c;
}

}

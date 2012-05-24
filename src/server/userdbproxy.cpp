#include "userdbproxy.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <assert.h>

#include "QsLog.h"
#include "sdcHelper.h"

namespace sdcs
{

#define CONNECTION ("sdcs")

/* Initialize static connection member. */
UserDbProxy::Connection UserDbProxy::connection;

UserDbProxy::Connection::~Connection()
{
    if (db.isOpen())
        db.close();
}

void UserDbProxy::Connection::open()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    if (db.isOpen())
        return;

    db = QSqlDatabase::addDatabase(driver, CONNECTION);
    db.setHostName(host);
    db.setDatabaseName(database);
    db.setUserName(user);
    db.setPassword(password);

    bool ok = db.open();
    if (!ok) {
        throw sdc::UserHandlingException(db.lastError().text().toStdString());
    }
}

QSharedPointer<UserDbProxy> UserDbProxy::getProxy(const QString &user)
throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    return QSharedPointer<UserDbProxy>(new UserDbProxy(user));
}

void UserDbProxy::saveContactList(const sdc::SecureContainer &/*container*/)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    throw sdc::LogException("Not implemented yet");
}

sdc::SecureContainer UserDbProxy::retrieveContactList()
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    throw sdc::LogException("Not implemented yet");
}

void UserDbProxy::saveLog(const QString &/*chatID*/, long /*timestamp*/, const sdc::SecureContainer &/*container*/)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    throw sdc::LogException("Not implemented yet");
}

sdc::Loglist UserDbProxy::retrieveLoglist()
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    throw sdc::LogException("Not implemented yet");
}

sdc::SecureContainer UserDbProxy::retrieveLog(const QString &/*chatID*/, long /*timestamp*/)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    throw sdc::LogException("Not implemented yet");
}

void UserDbProxy::setHost(const QString &host)
{
    connection.host = host;
}

void UserDbProxy::setDriver(const QString &driver)
{
    connection.driver = driver;
}

void UserDbProxy::setDatabase(const QString &database)
{
    connection.database = database;
}

void UserDbProxy::setUser(const QString &user)
{
    connection.user = user;
}

void UserDbProxy::setPassword(const QString &password)
{
    connection.password = password;
}

sdc::User UserDbProxy::getUser() const
{
    return user;
}

QByteArray UserDbProxy::getHash() const
{
    return hash;
}

QByteArray UserDbProxy::getSalt() const
{
    return salt;
}

void UserDbProxy::deleteUser()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QSqlQuery query(QSqlDatabase::database(CONNECTION));
    query.prepare("delete from public.user where username = :username");
    query.bindValue(":username", QString::fromStdString(user.ID));
    query.exec();
}

void UserDbProxy::createUser(sdc::User user, QByteArray hash, QByteArray salt)
throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    connection.open();

    QSqlQuery query(QSqlDatabase::database(CONNECTION));
    query.prepare("insert into public.user(username, public_key, password_hash, salt) "
                  "select :username, :public_key, :password_hash, :salt;");
    query.bindValue(":username", QString::fromStdString(user.ID));
    query.bindValue(":public_key", sdc::sdcHelper::byteSeqToByteArray(user.publicKey));
    query.bindValue(":password_hash", hash);
    query.bindValue(":salt", salt);

    bool ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::UserHandlingException(query.lastError().text().toStdString());
    }
}

UserDbProxy::UserDbProxy(const QString &username)
throw(sdc::UserHandlingException)
{
    connection.open();

    QSqlQuery query(QSqlDatabase::database(CONNECTION));
    query.prepare("select username, public_key, password_hash, salt, id "
                  "from public.user where username = :username");
    query.bindValue(":username", username);
    query.exec();

    if (query.size() == 0)
        throw sdc::UserHandlingException("User not found.");

    assert(query.size() == 1);
    query.first();

    user.ID = username.toStdString();
    user.publicKey = sdc::sdcHelper::byteArraytoByteSeq(query.value(1).toByteArray());
    hash = query.value(2).toByteArray();
    salt = query.value(3).toByteArray();
    id = query.value(4).toInt();
}

}

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

void UserDbProxy::saveContactList(const sdc::SecureContainer &container)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSqlQuery query(connection.database);
    query.prepare("delete from public.contactlist where user_id = :user_id;");
    query.bindValue(":user_id", id);

    bool ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::LogException(query.lastError().text().toStdString());
    }

    query.prepare("insert into public.contactlist (user_id, encrypted_content, signature) "
                  "select :user_id, :encrypted_content, :signature;");
    query.bindValue(":user_id", id);
    query.bindValue(":encrypted_content", sdc::sdcHelper::byteSeqToByteArray(container.data));
    query.bindValue(":signature", sdc::sdcHelper::byteSeqToByteArray(container.signature));

    ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::LogException(query.lastError().text().toStdString());
    }
}

sdc::SecureContainer UserDbProxy::retrieveContactList()
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSqlQuery query(connection.database);
    query.prepare("select encrypted_content, signature "
                  "from public.contactlist where user_id = :user_id;");
    query.bindValue(":user_id", id);

    bool ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::LogException(query.lastError().text().toStdString());
    }

    if (query.size() > 1) {
        QLOG_ERROR() << "Non-unique contact list query results";
        throw sdc::LogException("Non-unique contact list query results.");
    }

    sdc::SecureContainer container;

    if (query.size() == 1) {
        query.first();
        container.data = sdc::sdcHelper::byteArraytoByteSeq(query.value(0).toByteArray());
        container.signature = sdc::sdcHelper::byteArraytoByteSeq(query.value(1).toByteArray());
    }

    return container;
}

void UserDbProxy::saveLog(const QString &chatID, long timestamp, const sdc::SecureContainer &container)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSqlQuery query(connection.database);
    query.prepare("delete from public.chatlog where user_id = :user_id;");
    query.bindValue(":user_id", id);

    bool ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::LogException(query.lastError().text().toStdString());
    }

    query.prepare("insert into public.chatlog (user_id, chat_id, time_stamp, encrypted_content, signature) "
                  "select :user_id, :chat_id, :time_stamp, :encrypted_content, :signature;");
    query.bindValue(":user_id", id);
    query.bindValue(":chat_id", chatID);
    query.bindValue(":time_stamp", qlonglong(timestamp));
    query.bindValue(":encrypted_content", sdc::sdcHelper::byteSeqToByteArray(container.data));
    query.bindValue(":signature", sdc::sdcHelper::byteSeqToByteArray(container.signature));

    ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::LogException(query.lastError().text().toStdString());
    }
}

sdc::Loglist UserDbProxy::retrieveLoglist()
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSqlQuery query(connection.database);
    query.prepare("select chat_id, time_stamp "
                  "from public.chatlog where user_id = :user_id;");
    query.bindValue(":user_id", id);

    bool ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::LogException(query.lastError().text().toStdString());
    }

    sdc::Loglist list;

    while (query.next()) {
        sdc::ChatlogEntry entry;
        entry.chatID = query.value(0).toString().toStdString();
        entry.timestamp = static_cast<Ice::Long>(query.value(1).toLongLong());
        list.push_back(entry);
    }

    return list;
}

sdc::SecureContainer UserDbProxy::retrieveLog(const QString &chatID, long timestamp)
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QSqlQuery query(connection.database);
    query.prepare("select encrypted_content, signature "
                  "from public.chatlog where "
                  "user_id = :user_id and chat_id = :chat_id and time_stamp = :time_stamp;");
    query.bindValue(":user_id", id);
    query.bindValue(":chat_id", chatID);
    query.bindValue(":time_stamp", qlonglong(timestamp));

    bool ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::LogException(query.lastError().text().toStdString());
    }

    if (query.size() != 1) {
        QLOG_ERROR() << "Non-unique or nonexistent log query results";
        throw sdc::LogException("Non-unique or nonexistent log query results");
    }

    sdc::SecureContainer container;

    query.first();
    container.data = sdc::sdcHelper::byteArraytoByteSeq(query.value(0).toByteArray());
    container.signature = sdc::sdcHelper::byteArraytoByteSeq(query.value(1).toByteArray());

    return container;
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

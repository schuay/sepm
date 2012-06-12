/* *************************************************************************
 *  Copyright 2012 TU VIENNA SEPM GROUP 22                                 *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */


#include "userdbproxy.h"

#include <QMutexLocker>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <assert.h>

#include "QsLog.h"
#include "sdcHelper.h"
#include "settings.h"

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

    db = QSqlDatabase::addDatabase(sdc::Settings::getValue(sdc::Settings::SDbDriver).toString(),
                                   CONNECTION);
    db.setHostName(sdc::Settings::getValue(sdc::Settings::SDbHost).toString());
    db.setDatabaseName(sdc::Settings::getValue(sdc::Settings::SDbDatabase).toString());
    db.setUserName(sdc::Settings::getValue(sdc::Settings::SDbUser).toString());
    db.setPassword(sdc::Settings::getValue(sdc::Settings::SDbPassword).toString());

    bool ok = db.open();
    if (!ok) {
        throw sdc::UserHandlingException("Internal Server Error: Could not open database.\n" +
                                         db.lastError().text().toStdString());
    }
}

QSharedPointer<UserDbProxy> UserDbProxy::getProxy(const QString &user)
throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    return QSharedPointer<UserDbProxy>(new UserDbProxy(user));
}

void UserDbProxy::saveContactList(const sdc::SecureContainer &container)
throw(sdc::ContactException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QMutexLocker lock(&connection.dbMutex);

    QSqlQuery query(connection.db);
    query.prepare("delete from public.contactlist where user_id = :user_id;");
    query.bindValue(":user_id", id);

    bool ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::ContactException("Internal Server Error: Could not delete old contact list.\n"
                                    + query.lastError().text().toStdString());
    }

    query.prepare("insert into public.contactlist (user_id, encrypted_content, signature) "
                  "select :user_id, :encrypted_content, :signature;");
    query.bindValue(":user_id", id);
    query.bindValue(":encrypted_content", sdc::sdcHelper::byteSeqToByteArray(container.data));
    query.bindValue(":signature", sdc::sdcHelper::byteSeqToByteArray(container.signature));

    ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::ContactException("Internal Server Error: Could not save contact list.\n"
                                    + query.lastError().text().toStdString());
    }
}

sdc::SecureContainer UserDbProxy::retrieveContactList()
throw(sdc::ContactException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QMutexLocker lock(&connection.dbMutex);

    QSqlQuery query(connection.db);
    query.prepare("select encrypted_content, signature "
                  "from public.contactlist where user_id = :user_id;");
    query.bindValue(":user_id", id);

    bool ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::ContactException(query.lastError().text().toStdString());
    }

    if (query.size() > 1) {
        QLOG_ERROR() << "Non-unique contact list query results";
        throw sdc::ContactException("Internal Server Error: Non-unique contact list query results.");
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

    QMutexLocker lock(&connection.dbMutex);

    QSqlQuery query(connection.db);
    query.prepare("delete from public.chatlog where user_id = :user_id and chat_id = :chat_id "
                  "and time_stamp = :time_stamp;");
    query.bindValue(":user_id", id);
    query.bindValue(":chat_id", chatID);
    query.bindValue(":time_stamp", qlonglong(timestamp));

    bool ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::LogException("Internal Server Error: Could not delete old log.\n"
                                + query.lastError().text().toStdString());
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
        throw sdc::LogException("Internal Server Error: Could not save log.\n"
                                + query.lastError().text().toStdString());
    }
}

sdc::Loglist UserDbProxy::retrieveLoglist()
throw(sdc::LogException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QMutexLocker lock(&connection.dbMutex);

    QSqlQuery query(connection.db);
    query.prepare("select chat_id, time_stamp "
                  "from public.chatlog where user_id = :user_id "
                  "order by time_stamp desc, chat_id;");

    query.bindValue(":user_id", id);

    bool ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::LogException("Internal Server Error: Could not retrieve loglist\n"
                                + query.lastError().text().toStdString());
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

    QMutexLocker lock(&connection.dbMutex);

    QSqlQuery query(connection.db);
    query.prepare("select encrypted_content, signature "
                  "from public.chatlog where "
                  "user_id = :user_id and chat_id = :chat_id and time_stamp = :time_stamp;");
    query.bindValue(":user_id", id);
    query.bindValue(":chat_id", chatID);
    query.bindValue(":time_stamp", qlonglong(timestamp));

    bool ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::LogException("Internal Server Error: Could not retrieve log.\n"
                                + query.lastError().text().toStdString());
    }

    if (query.size() != 1) {
        QLOG_ERROR() << "Non-unique or nonexistent log query results";
        throw sdc::LogException("Internal Server Error: Non-unique or nonexistent log query results");
    }

    sdc::SecureContainer container;

    query.first();
    container.data = sdc::sdcHelper::byteArraytoByteSeq(query.value(0).toByteArray());
    container.signature = sdc::sdcHelper::byteArraytoByteSeq(query.value(1).toByteArray());

    return container;
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
throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QMutexLocker lock(&connection.dbMutex);

    QSqlQuery query(QSqlDatabase::database(CONNECTION));

    query.prepare("delete from public.chatlog where user_id = :user_id");
    query.bindValue(":user_id", id);

    bool ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::UserHandlingException("Internal Server Error: Could not delete chatlog\n"
                                         + query.lastError().text().toStdString());
    }

    query.prepare("delete from public.contactlist where user_id = :user_id");
    query.bindValue(":user_id", id);

    ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::UserHandlingException("Internal Server Error: Could not delete contact list\n"
                                         + query.lastError().text().toStdString());
    }

    query.prepare("delete from public.user where username = :username");
    query.bindValue(":username", QString::fromStdString(user.ID));

    ok = query.exec();
    if (!ok) {
        QLOG_ERROR() << query.lastError().text();
        throw sdc::UserHandlingException("Internal Server Error: Could not delete user\n"
                                         + query.lastError().text().toStdString());
    }
}

void UserDbProxy::createUser(sdc::User user, QByteArray hash, QByteArray salt)
throw(sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;

    QMutexLocker lock(&connection.dbMutex);

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

        QLOG_ERROR() << query.lastError().text() << "(User probably exists)";

        /* Try to give a more helpful error  message. However, Due to a bug in QPSQL,
         * the PostgreSQL error number is not actually set in query.lastError().number(),
         * so we can only switch on the type. The most important error case is that
         * a user already exists. We would expect that to be a TransactionError,
         * it is reported as StatementError (which indicates a syntax error actually). */
        switch (query.lastError().type()) {
        case QSqlError::StatementError:     /* What is reported. */
        case QSqlError::TransactionError:   /* Just in case future QPSQL versions get it right. */
            throw sdc::UserHandlingException("User already exists");

        default: /* For example, lost connection. */
            throw sdc::UserHandlingException(query.lastError().text().toStdString());
        }
    }
}

UserDbProxy::UserDbProxy(const QString &username)
throw(sdc::UserHandlingException)
{
    QMutexLocker lock(&connection.dbMutex);

    connection.open();

    QSqlQuery query(QSqlDatabase::database(CONNECTION));
    query.prepare("select username, public_key, password_hash, salt, id "
                  "from public.user where username = :username");
    query.bindValue(":username", username);
    query.exec();

    if (query.size() == 0)
        throw NoSuchUserException("User not found.");

    assert(query.size() == 1);
    query.first();

    user.ID = username.toStdString();
    user.publicKey = sdc::sdcHelper::byteArraytoByteSeq(query.value(1).toByteArray());
    hash = query.value(2).toByteArray();
    salt = query.value(3).toByteArray();
    id = query.value(4).toInt();
}

}

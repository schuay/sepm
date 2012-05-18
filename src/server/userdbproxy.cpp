#include "userdbproxy.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <assert.h>

#include "QsLog.h"

namespace sdcs
{

#define CONNECTION ("sdcs")

/* Initialize static connection member. */
UserDbProxy::Connection UserDbProxy::connection;

UserDbProxy::Connection::Connection()
{
    /* Default values. */
    driver = "QPSQL";
    host = "127.0.0.41";
    database = "grp22";
    user = "grp22";
    password = "se20linux901";
}

UserDbProxy::Connection::~Connection()
{
    if (db.isOpen())
        db.close();
}

void UserDbProxy::Connection::open()
{
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
throw (sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    return QSharedPointer<UserDbProxy>(new UserDbProxy(user));
}

sdc::ByteSeq UserDbProxy::toByteSeq(const QByteArray &array)
{
    const unsigned char *p = reinterpret_cast<const unsigned char *>(array.data());
    return sdc::ByteSeq(p, p + array.size());
}

QByteArray UserDbProxy::fromByteSeq(const sdc::ByteSeq &seq)
{
    const char *p = reinterpret_cast<const char *>(&seq[0]);
    return QByteArray(p, seq.size());
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

sdc::ByteSeq UserDbProxy::getHash() const
{
    return hash;
}

void UserDbProxy::deleteUser()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QSqlQuery query(QSqlDatabase::database(CONNECTION));
    query.prepare("delete from public.user where username = :username");
    query.bindValue(":username", QString::fromStdString(user.ID));
    query.exec();
}

void UserDbProxy::createUser(sdc::User user, sdc::ByteSeq hash)
throw (sdc::UserHandlingException)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    QSqlQuery query(QSqlDatabase::database(CONNECTION));
    query.prepare("insert into public.user(username, public_key, password_hash) "
                  "select :username, :public_key, :password_hash;");
    query.bindValue(":username", QString::fromStdString(user.ID));
    query.bindValue(":public_key", fromByteSeq(user.publicKey));
    query.bindValue(":password_hash", fromByteSeq(hash));

    bool ok = query.exec();
    if (!ok)
        throw sdc::UserHandlingException(query.lastError().text().toStdString());
}

UserDbProxy::UserDbProxy(const QString &username)
throw (sdc::UserHandlingException)
{
    connection.open();

    QSqlQuery query(QSqlDatabase::database(CONNECTION));
    query.prepare("select username, public_key, password_hash "
                  "from public.user where username = :username");
    query.bindValue(":username", username);
    query.exec();

    if (query.size() == 0)
        throw sdc::UserHandlingException("User not found.");

    assert(query.size() == 1);
    query.first();

    user.ID = username.toStdString();
    user.publicKey = toByteSeq(query.value(1).toByteArray());
    hash = toByteSeq(query.value(2).toByteArray());
}

}

#include "userdbproxy.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <assert.h>

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
        throw UserHandlingException(db.lastError().text().toStdString());
    }
}

QSharedPointer<UserDbProxy> UserDbProxy::getProxy(const QString &user)
throw (UserHandlingException)
{
    return QSharedPointer<UserDbProxy>(new UserDbProxy(user));
}

sdc::ByteSeq UserDbProxy::toByteSeq(const QByteArray &array)
{
    const unsigned char *p = reinterpret_cast<const unsigned char *>(array.data());
    return sdc::ByteSeq(p, p + array.size());
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
    QSqlQuery query(QSqlDatabase::database(CONNECTION));
    query.prepare("delete from public.user where username = :username");
    query.bindValue(":username", QString::fromStdString(user.ID));
    query.exec();
}

void UserDbProxy::createUser(User user, ByteSeq hash)
{
    Q_UNUSED(user);
    Q_UNUSED(hash);
}

UserDbProxy::UserDbProxy(const QString &username)
throw (UserHandlingException)
{
    connection.open();

    QSqlQuery query(QSqlDatabase::database(CONNECTION));
    query.prepare("select username, public_key, password_hash "
                  "from public.user where username = :username");
    query.bindValue(":username", username);
    query.exec();

    if (query.size() == 0)
        throw UserHandlingException("User not found.");

    assert(query.size() == 1);
    query.first();

    user.ID = username.toStdString();
    user.publicKey = toByteSeq(query.value(1).toByteArray());
    hash = toByteSeq(query.value(2).toByteArray());
}

}

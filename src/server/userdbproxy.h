#ifndef USERDBPROXY_H
#define USERDBPROXY_H

#include <QString>
#include <QSharedPointer>
#include <QSqlDatabase>

#include "SecureDistributedChat.h"

namespace sdcs
{

/**
 * @brief The UserDbProxy class is the access point to user related
 *        data stored in the backend database.
 */
class UserDbProxy
{
private:
    struct Connection {
        ~Connection();

        void open();

        QSqlDatabase db;
        QString driver, host, database, user, password;
    };

public:
    /**
     * Adds the user with the given salted password hash to the database.
     */
    static void createUser(sdc::User user, QByteArray hash, QByteArray salt)
    throw(sdc::UserHandlingException);

    /**
     * Returns a UserDbProxy instance which can be used to query and manipulate
     * information of the specified user. Throws a UserHandlingException if the
     * the user could not be retrieved.
     */
    static QSharedPointer<UserDbProxy> getProxy(const QString &user)
    throw(sdc::UserHandlingException);

    sdc::User getUser() const;
    QByteArray getHash() const;
    QByteArray getSalt() const;

    /**
     * Deletes the user and all related information from the database.
     * Accesses to this object are invalid after completion of this method.
     */
    void deleteUser();

    /**
     * Sets the database hostname. Must be set before the first UserDbProxy instance
     * is created.
     */
    static void setHost(const QString &host);

    /**
     * Sets the database driver. Must be set before the first UserDbProxy instance
     * is created.
     */
    static void setDriver(const QString &driver);

    /**
     * Sets the database. Must be set before the first UserDbProxy instance
     * is created.
     */
    static void setDatabase(const QString &database);

    /**
     * Sets the database user. Must be set before the first UserDbProxy instance
     * is created.
     */
    static void setUser(const QString &user);

    /**
     * Sets the database password. Must be set before the first UserDbProxy instance
     * is created.
     */
    static void setPassword(const QString &password);

private:
    UserDbProxy(const QString &user) throw(sdc::UserHandlingException);
    UserDbProxy(const UserDbProxy &);

    sdc::User user;
    QByteArray hash;
    QByteArray salt;


    /**
     * Wraps the QSqlDatabase object and closes it automatically on destruction.
     */
    static Connection connection;
};

}

#endif

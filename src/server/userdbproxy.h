#ifndef USERDBPROXY_H
#define USERDBPROXY_H

#include <QString>
#include <QSharedPointer>
#include <QSqlDatabase>

#include "SecureDistributedChat.h"

namespace sdcs
{

/**
 * @brief This Exception is used if a specific user was not found in the
 * 	  database.
 */
class NoSuchUserException : public sdc::UserHandlingException
{
public:
    NoSuchUserException(const char *msg) : sdc::UserHandlingException(msg) {}
};

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
    void deleteUser()
    throw(sdc::UserHandlingException);

    /**
     * Saves the contact list in the database, overwriting any existing contact list.
     */
    void saveContactList(const sdc::SecureContainer &container)
    throw(sdc::ContactException);

    /**
     * Retrieves the contact list. If no contact list is saved, returns an empty container.
     */
    sdc::SecureContainer retrieveContactList()
    throw(sdc::ContactException);

    /**
     * Saves a log in the database with the given chatID and timestamp.
     * If the log already exists, it is updated with the new contents.
     */
    void saveLog(const QString &chatID, long timestamp, const sdc::SecureContainer &container)
    throw(sdc::LogException);

    /**
     * Retrieves a list of all stored logs.
     */
    sdc::Loglist retrieveLoglist()
    throw(sdc::LogException);

    /**
     * Retrieves a specific log. Throws a LogException if it doesn't exist.
     */
    sdc::SecureContainer retrieveLog(const QString &chatID, long timestamp)
    throw(sdc::LogException);

private:
    UserDbProxy(const QString &user) throw(sdc::UserHandlingException);
    UserDbProxy(const UserDbProxy &);

    int id;
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

#include "usermodel.h"
#include "sdcHelper.h"
#include "session.h"
#include <QStringListIterator>
#include <QsLog.h>

namespace sdcc
{
UserModel::UserModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

UserModel::UserModel(QSharedPointer<Session> session) :
    QAbstractListModel(0)
{
    d_session = session;
    connect(d_session.data(),
            SIGNAL(retrieveUserCompleted(QSharedPointer<const User>, const QObject *, bool, const QString)),
            this,
            SLOT(onUserRetrieved(QSharedPointer<const User>, const QObject *, bool, QString)));
}

int UserModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;

    return d_users.count();
}

QVariant UserModel::data(const QModelIndex &index, int role) const
{
    QSharedPointer<const User> user = d_users.values().at(index.row());

    if (role == Qt::DisplayRole) {
        QString name = QString(
                           sdc::sdcHelper::getNameFromID(
                               user->getName().toStdString()
                           ).c_str()
                       );
        return QVariant(name);
    }

    if (role == Qt::ToolTipRole) {
        return QVariant(user->getName());
    }

    return QVariant();
}

QSharedPointer<const User> UserModel::getUser(const QString &username) const
{
    return d_users.value(username);
}

QSharedPointer<const User> UserModel::getUser(int id) const
{
    return d_users.values().at(id);
}

QSharedPointer<const User> UserModel::getUser(const QModelIndex &index) const
{
    return getUser(index.row());
}

void UserModel::addUser(QSharedPointer<const User> user)
{
    if (!d_users.contains(user->getName())) {
        d_users.insert(user->getName(), user);
        int row = d_users.values().indexOf(user);

        emit dataChanged(
            createIndex(row, 0),
            createIndex(rowCount(), 0)
        );
    }
}

QSharedPointer<const User> UserModel::removeUser(const QString &username)
{
    if (d_users.contains(username)) {
        QSharedPointer<const User> user = d_users.value(username);
        int row = d_users.keys().indexOf(username);
        d_users.remove(username);
        emit dataChanged(
            createIndex(row, 0),
            createIndex(rowCount(), 0)
        );
        return user;
    }

    return QSharedPointer<const sdcc::User>();
}

QSharedPointer<const User> UserModel::removeUser(QSharedPointer<const User> user)
{
    return removeUser(user->getName());
}

QSharedPointer<const User> UserModel::removeUser(int index)
{
    return removeUser(d_users.keys().at(index));
}

QSharedPointer<const User> UserModel::removeUser(const QModelIndex &index)
{
    return removeUser(index.row());
}

void UserModel::fromNameList(const QStringList &userList, QSharedPointer<Session> session)
{
    QStringListIterator it(userList);
    if (session.isNull()) {
        session = d_session;
    } else {
        connect(session.data(),
                SIGNAL(retrieveUserCompleted(QSharedPointer<const User>, QObject *, bool, const QString)),
                this,
                SLOT(onUserRetrieved(QSharedPointer<const User>, QObject *, bool, QString)));
    }
    while (it.hasNext()) {
        QString name = it.next();
        session->retrieveUser(name, this);
    }
}

const QStringList UserModel::toUserList() const
{
    return QStringList(d_users.keys());
}

void UserModel::onUserRetrieved(QSharedPointer<const User> user, const QObject *id, bool success, const QString &)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (success && id == this) {
        addUser(user);
    }
}

}

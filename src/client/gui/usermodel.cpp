#include "usermodel.h"
#include "sdcHelper.h"


namespace sdcc
{
UserModel::UserModel(QObject *parent) :
    QAbstractListModel(parent)
{
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

}

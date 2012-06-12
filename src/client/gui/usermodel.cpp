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


#include "usermodel.h"
#include "sdcHelper.h"
#include "session.h"
#include <QStringListIterator>
#include <QsLog.h>
#include <QMessageBox>

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
    QSharedPointer<const User> user = getUser(index);
    if (user.isNull())
        return QVariant();

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
    if (id < d_users.count() && id >= 0)
        return d_users.values().at(id);
    else
        return QSharedPointer<const sdcc::User>();
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
    if (index < d_users.count() && index >= 0)
        return removeUser(d_users.keys().at(index));
    else
        return QSharedPointer<const sdcc::User>();
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

void UserModel::onUserRetrieved(QSharedPointer<const User> user, const QObject *id, bool success, const QString &msg)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (id != this) return;
    if (success) {
        addUser(user);
    } else {
        QMessageBox::warning(0, "Couldn't add user to contact list", msg);
    }
}

}

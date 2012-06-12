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


#ifndef USERMODEL_H
#define USERMODEL_H

#include <QAbstractListModel>
#include <QSharedPointer>
#include <QMap>
#include <QString>
#include "user.h"
#include "session.h"

namespace sdcc
{

class UserModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit UserModel(QObject *parent = 0);
    explicit UserModel(QSharedPointer<Session> session);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QSharedPointer<const User> getUser(const QString &username) const;
    QSharedPointer<const User> getUser(int id) const;
    QSharedPointer<const User> getUser(const QModelIndex &index) const;
    void addUser(QSharedPointer<const User> user);
    QSharedPointer<const User> removeUser(const QString &username);
    QSharedPointer<const User> removeUser(QSharedPointer<const User> user);
    QSharedPointer<const User> removeUser(int index);
    QSharedPointer<const User> removeUser(const QModelIndex &index);
    void fromNameList(const QStringList &userList, QSharedPointer<Session> session = QSharedPointer<Session>());
    const QStringList toUserList() const;
signals:

public slots:
    void onUserRetrieved(QSharedPointer<const User> user, const QObject *id, bool success, const QString &msg);

private:
    QMap<QString, QSharedPointer<const User> > d_users;
    QSharedPointer<Session> d_session;

};
}

#endif // USERMODEL_H

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

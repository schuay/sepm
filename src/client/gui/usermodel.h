#ifndef USERMODEL_H
#define USERMODEL_H

#include <QAbstractListModel>
#include <QSharedPointer>
#include <QMap>
#include <QString>
#include "user.h"

namespace sdcc
{

class UserModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit UserModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QSharedPointer<const User> getUser(const QString &username) const;
    QSharedPointer<const User> getUser(int id) const;
    void addUser(QSharedPointer<const User> user);
    QSharedPointer<const User> removeUser(const QString &username);
    QSharedPointer<const User> removeUser(QSharedPointer<const User> user);
    QSharedPointer<const User> removeUser(int index);

signals:

public slots:

private:
    QMap<QString, QSharedPointer<const User> > d_users;

};
}

#endif // USERMODEL_H

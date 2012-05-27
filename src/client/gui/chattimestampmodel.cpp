#include "chattimestampmodel.h"
#include <QtCore>

ChatTimeStampModel::ChatTimeStampModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant ChatTimeStampModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        QString date = QString(modeldata.at(index.row()).first.toString());
        QString chatname = QString(modeldata.at(index.row()).second);
        return QString(date + " " + chatname);
    }
    if (role == 2) {
        return QVariant(modeldata.at(index.row()).first);
    }
    if (role == 1) {
        return QVariant(modeldata.at(index.row()).second);
    }
    return QVariant();
}

void ChatTimeStampModel::setEntries(const QList<QPair<QDateTime, QString> > &entries)
{
    modeldata = entries;
    reset();
}

int ChatTimeStampModel::rowCount(const QModelIndex &parent = QModelIndex()) const
{
    Q_UNUSED(parent);
    return modeldata.count();
}

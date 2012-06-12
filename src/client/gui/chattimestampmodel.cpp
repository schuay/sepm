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

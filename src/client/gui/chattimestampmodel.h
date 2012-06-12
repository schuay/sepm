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


#ifndef CHATTIMESTAMPMODEL_H
#define CHATTIMESTAMPMODEL_H
#include <QtCore>

class ChatTimeStampModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ChatTimeStampModel(QObject *parent = 0);
    void setEntries(const QList<QPair<QDateTime, QString> > &entries);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;

private:
    QList<QPair<QDateTime, QString> > modeldata;
};

#endif // CHATTIMESTAMPMODEL_H

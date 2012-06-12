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


#ifndef CHATLOG_H
#define CHATLOG_H

#include <QDialog>
#include <session.h>
#include <QtGui>
#include "chattimestampmodel.h"

using namespace sdcc;

namespace Ui
{
class ChatLogDialog;
}

class ChatLogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatLogDialog(QWidget *parent = 0, QSharedPointer<Session> session
                           = QSharedPointer<Session>());
    ~ChatLogDialog();

public slots:
    void onRetrieveLoglistCompleted(const QList<QPair<QDateTime, QString> > &list,
                                    bool success, const QString &msg);
    void onTimeStampClicked(QModelIndex index);
    void onRetrieveLogCompleted(const QList<ChatlogEntry> &list,
                                bool success, const QString &msg);
    void selectionChanged(const QItemSelection &sel, const QItemSelection &des);
private:
    Ui::ChatLogDialog *ui;
    QSharedPointer<Session> d_session;
    QList<QPair<QDateTime, QString> > chatlogtimestamps;
    ChatTimeStampModel *chattimestampmodel;
};

#endif // CHATLOG_H

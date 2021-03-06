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


#include "chatlogdialog.h"
#include "ui_chatlogdialog.h"
#include <QtGui>

ChatLogDialog::ChatLogDialog(QWidget *parent, QSharedPointer<Session> session) :
    QDialog(parent),
    ui(new Ui::ChatLogDialog)
{
    qRegisterMetaType<QList<QPair<QDateTime, QString> > >("QList<QPair<QDateTime, QString> >");
    qRegisterMetaType<QList<ChatlogEntry> >("QList<ChatlogEntry>");

    d_session = session;
    chattimestampmodel = new ChatTimeStampModel();
    d_session->retrieveLoglist();
    ui->setupUi(this);
    this->setWindowTitle("View Chatlogs");
    setWindowIcon(QIcon(":/appicon.png"));

    connect(d_session.data(),
            SIGNAL(retrieveLoglistCompleted(const QList<QPair<QDateTime, QString> >, bool, const QString)),
            this,
            SLOT(onRetrieveLoglistCompleted(const QList<QPair<QDateTime, QString> >, bool, const QString)));

    connect(ui->lwChatTimestampList,
            SIGNAL(activated(QModelIndex)),
            this,
            SLOT(onTimeStampClicked(QModelIndex)));

    connect(d_session.data(),
            SIGNAL(retrieveLogCompleted(const QList<ChatlogEntry>, bool, const QString)),
            this,
            SLOT(onRetrieveLogCompleted(const QList<ChatlogEntry>, bool, QString)));

    connect(ui->closeButton, SIGNAL(rejected()), this, SLOT(close()));
    this->setModal(true);
}

void ChatLogDialog::selectionChanged(const QItemSelection &sel, const QItemSelection &des)
{
    Q_UNUSED(des);
    onTimeStampClicked(sel.indexes().at(0));

}

static bool compareByDateDesc(QPair<QDateTime, QString> item1, QPair<QDateTime, QString> item2)
{
    return item1.first > item2.first;
}

void ChatLogDialog::onRetrieveLoglistCompleted(QList<QPair<QDateTime, QString> > list,
        bool success, const QString &msg)
{
    if (success) {
        qSort(list.begin(), list.end(), &compareByDateDesc);

        chattimestampmodel->setEntries(list);
        ui->lwChatTimestampList->setModel(chattimestampmodel);

        // display the contents of the first entry
        if (list.size() > 0) {
            d_session->retrieveLog(list.at(0).first, list.at(0).second);
        }

        // We can only connect the signal after a model has been assigned.
        connect(ui->lwChatTimestampList->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));

    } else {
        QMessageBox::warning(this, "Retrieve Loglist Failed", msg);
    }

}

void ChatLogDialog::onTimeStampClicked(QModelIndex index)
{
    QDateTime date = ui->lwChatTimestampList->model()->data(index, 2).toDateTime();
    QString chatname = ui->lwChatTimestampList->model()->data(index, 1).toString();
    d_session->retrieveLog(date, chatname);
}

void ChatLogDialog::onRetrieveLogCompleted(QList<ChatlogEntry> list, bool success, const QString &msg)
{
    if (success) {
        QStringList temp;
        for (int i = 0; i < list.count(); i++) {
            temp.append(list.at(i).getTimestamp().toString() + " " + list.at(i).getSender() + " " + list.at(i).getMessage());
        }
        ui->lwChatContent->clear();
        ui->lwChatContent->addItems(temp);
    } else {
        QMessageBox::warning(this, "No Logs", msg);
    }
}

ChatLogDialog::~ChatLogDialog()
{
    delete ui;
    delete chattimestampmodel;
}

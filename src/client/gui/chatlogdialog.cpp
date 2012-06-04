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
    connect(d_session.data(),
            SIGNAL(retrieveLoglistCompleted(const QList<QPair<QDateTime, QString> >, bool, const QString)),
            this,
            SLOT(onRetrieveLoglistCompleted(const QList<QPair<QDateTime, QString> >, bool, const QString)));

    connect(ui->lwChatTimestampList,
            SIGNAL(activated(QModelIndex)),
            this,
            SLOT(onTimeStampClicked(QModelIndex)));

    connect(ui->lwChatTimestampList,
            SIGNAL(pressed(QModelIndex)),
            this,
            SLOT(onTimeStampClicked(QModelIndex)));

    connect(d_session.data(),
            SIGNAL(retrieveLogCompleted(const QList<ChatlogEntry>, bool, const QString)),
            this,
            SLOT(onRetrieveLogCompleted(const QList<ChatlogEntry>, bool, QString)));


}

void ChatLogDialog::selectionChanged(const QItemSelection &sel, const QItemSelection &des)
{
    Q_UNUSED(des);
    onTimeStampClicked(sel.indexes().at(0));

}

void ChatLogDialog::onRetrieveLoglistCompleted(const QList<QPair<QDateTime, QString> > &list,
        bool success, const QString &msg)
{
    if (success) {
        chattimestampmodel->setEntries(list);
        ui->lwChatTimestampList->setModel(chattimestampmodel);
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

void ChatLogDialog::onRetrieveLogCompleted(const QList<ChatlogEntry> &list, bool success, const QString &msg)
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

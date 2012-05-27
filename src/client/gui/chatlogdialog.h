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
private:
    Ui::ChatLogDialog *ui;
    QSharedPointer<Session> d_session;
    QList<QPair<QDateTime, QString> > chatlogtimestamps;
    ChatTimeStampModel *chattimestampmodel;
};

#endif // CHATLOG_H

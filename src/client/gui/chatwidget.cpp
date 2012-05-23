#include "chatwidget.h"
#include "ui_chatwidget.h"
#include "sdcHelper.h"
#include <iostream>
#include <QtGui>
#include <QsLog.h>

ChatWidget::ChatWidget(QSharedPointer<Session> session,
                       QSharedPointer<Chat> chat, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWidget)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);

    qRegisterMetaType<QSharedPointer<const User> >("QSharedPointer<const User>");

    d_chat = chat;
    d_session = session;

    QListIterator<QSharedPointer<const User> > it(d_chat->getUserList());
    while(it.hasNext()) {
        userJoined(it.next());
    }
    connect(ui->leMessage, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    connect(d_chat.data(), SIGNAL(messageReceived(QSharedPointer<const User>, QString)), this,
            SLOT(messageReceived(QSharedPointer<const User>, QString)));
    connect(d_chat.data(), SIGNAL(userJoined(QSharedPointer<const User>)), this, SLOT(userJoined(QSharedPointer<const User>)));
    connect(d_chat.data(), SIGNAL(userLeft(QSharedPointer<const User>)), this, SLOT(userLeft(QSharedPointer<const User>)));
    connect(d_chat.data(), SIGNAL(leaveChatCompleted(bool, QString)), this, SLOT(leaveChatCompleted(bool, QString)));
    connect(d_chat.data(), SIGNAL(inviteCompleted(bool, QString)), this, SLOT(inviteCompleted(bool, QString)));
    connect(d_chat.data(), SIGNAL(sendCompleted(bool, QString)), this, SLOT(sendCompleted(bool, QString)));
    qRegisterMetaType<QObject *>("QObject*");
    connect(d_session.data(), SIGNAL(retrieveUserCompleted(QSharedPointer<const User>, const QObject *, bool, QString)),
            this, SLOT(invite(QSharedPointer<const User>, const QObject *, bool, QString)));
}

ChatWidget::ChatWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWidget)
{
    ui->setupUi(this);
    ui->leMessage->setEnabled(false);
    ui->lwParticipants->setEnabled(false);
    ui->tbChat->setEnabled(false);
}

void ChatWidget::returnPressed()
{
    d_chat->send(ui->leMessage->text());
    ui->leMessage->setText("");
}

/**
 * Another user sent a message to the chat.
 */
void ChatWidget::messageReceived(QSharedPointer<const User> user, const QString &msg)
{
    ui->tbChat->append(user->getName() + QString(": ") + msg);
}

/**
 * Another user joined the chat.
 */
void ChatWidget::userJoined(QSharedPointer<const User> user)
{
    if(ui->lwParticipants->findItems(user->getName(), Qt::MatchExactly).size() == 0) {
        ui->lwParticipants->addItem(user->getName());
        ui->lwParticipants->sortItems();
    } else {
        QLOG_WARN() << "User " << user->getName() << " joined while already in chat.";
    }
}

void ChatWidget::userLeft(QSharedPointer<const User> user)
{
    QListWidgetItem *it = ui->lwParticipants->findItems(
                              user->getName(),
                              Qt::MatchExactly
                          ).first();

    if (it == NULL) {
        QLOG_ERROR() << QString("User '%1' not in participant list.").arg(user->getName());
        return;
    }

    ui->lwParticipants->takeItem(ui->lwParticipants->row(it));

    delete it;
}

/**
 * Response signal for leaveChat.
 */
void ChatWidget::leaveChatCompleted(bool success, const QString &msg)
{
    if (!success) {
        QMessageBox::warning(this, "Close failed on Server",
                             "The server couldn't close the chat. Please tell the developers of this bug.\n" + msg);
    }
    emit leftChat();
    this->close();
}

/**
 * Response signal for invite.
 */
void ChatWidget::inviteCompleted(bool success, const QString &msg)
{
    if (!success) {
        QMessageBox::warning(this, "Couldn't invite User",
                             "The server couldn't invite the user you requested into the chat.\n" + msg);
    }
}

/**
  * Invites a user. The Format
  */
void ChatWidget::invite(QSharedPointer<const User> user, const QObject *id, bool success, const QString &msg)
{
    if (id != this)
        return;
    if (success) {
        d_chat->invite(user);
    } else {
        QMessageBox::warning(this, "Couldn't find User", msg);
    }
}

void ChatWidget::leaveChat()
{
    d_chat->leaveChat();
}

/**
 * Response signal for send.
 */
void ChatWidget::sendCompleted(bool success, const QString &msg)
{
    if (!success) {
        QMessageBox::warning(this, "Failed to Send",
                             "The message didn't reach the other participants.\n" + msg);
    }
}

ChatWidget::~ChatWidget()
{
    delete ui;
}

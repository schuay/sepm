#include "chatwidget.h"
#include "ui_chatwidget.h"
#include <iostream>
#include <QtGui>

ChatWidget::ChatWidget(QSharedPointer<Session> session,
                       QSharedPointer<Chat> chat, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWidget)
{
    ui->setupUi(this);

    qRegisterMetaType<User>("User");

    d_chat = chat;
    this->session = session;

    connect(ui->leMessage, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    connect(d_chat.data(), SIGNAL(messageReceived(User, QString)), this, SLOT(messageReceived(User, QString)));
    connect(d_chat.data(), SIGNAL(userJoined(User)), this, SLOT(userJoined(User)));
    connect(d_chat.data(), SIGNAL(leaveChatCompleted(bool, QString)), this, SLOT(leaveChatCompleted(bool,QString)));
    connect(d_chat.data(), SIGNAL(inviteCompleted(bool, QString)), this, SLOT(inviteCompleted(bool, QString)));
    connect(d_chat.data(), SIGNAL(sendCompleted(bool, QString)), this, SLOT(sendCompleted(bool, QString)));
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
    lastMsg = ui->leMessage->text();
    d_chat->send(lastMsg);
    ui->leMessage->setText("");
}

/**
 * Another user sent a message to the chat.
 */
void ChatWidget::messageReceived(const User &user, const QString &msg)
{
    ui->tbChat->append(user.getName() + ": "+ msg);
}

/**
 * Another user joined the chat.
 */
void ChatWidget::userJoined(const User &user)
{
    QListWidgetItem userWidget(user.getName(), ui->lwParticipants);
}

/**
 * Response signal for leaveChat.
 */
void ChatWidget::leaveChatCompleted(bool success, const QString &msg)
{
    if(!success) {
        QMessageBox::warning(this, "Close failed on Server",
                             "The server couldn't close the chat. Please tell the developers of this bug.\n" + msg);
    }

    this->close();
    this->destroy();
}

/**
 * Response signal for invite.
 */
void ChatWidget::inviteCompleted(bool success, const QString &msg)
{
    if(!success) {
        QMessageBox::warning(this, "Couldn't invite User",
                             "The server couldn't invite the user you requested into the chat.\n" + msg);
    }
}

/**
  * Invites a user. The Format
  */
void ChatWidget::invite(QSharedPointer<User> user)
{
    d_chat->invite(*(user.data()));
}

/**
 * Response signal for send.
 */
void ChatWidget::sendCompleted(bool success, const QString &msg)
{
    if(!success) {
        QMessageBox::warning(this, "Failed to Send",
                             "The message didn't reach the other participants.\n" + msg);
    } else {
        messageReceived(*(session->getUser().data()), lastMsg);
    }
}


ChatWidget::~ChatWidget()
{
    delete ui;
}

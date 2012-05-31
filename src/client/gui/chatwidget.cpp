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
    pList = new UserModel(d_session);

    ui->lvParticipants->setModel(pList);

    QListIterator<QSharedPointer<const User> > it(d_chat->getUserList());
    while (it.hasNext()) {
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
    ui->lvParticipants->setEnabled(false);
    ui->tbChat->setEnabled(false);
}

void ChatWidget::returnPressed()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    d_chat->send(ui->leMessage->text());
    ui->leMessage->setText("");
}

/**
 * Another user sent a message to the chat.
 */
void ChatWidget::messageReceived(QSharedPointer<const User> user, const QString &msg)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    ui->tbChat->append(user->getName() + QString(": ") + msg);
    emit chatUpdate(this);
}

/**
 * Another user joined the chat.
 */
void ChatWidget::userJoined(QSharedPointer<const User> user)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    pList->addUser(user);
    int oldWeight = ui->tbChat->fontWeight();
    ui->tbChat->setFontWeight(QFont::Bold);
    ui->tbChat->append(user->getName() + QString(" joined"));
    ui->tbChat->setFontWeight(oldWeight);
    emit chatUpdate(this);
}

void ChatWidget::userLeft(QSharedPointer<const User> user)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    pList->removeUser(user);
    int oldWeight = ui->tbChat->fontWeight();
    ui->tbChat->setFontWeight(QFont::Bold);
    ui->tbChat->append(user->getName() + QString(" left"));
    ui->tbChat->setFontWeight(oldWeight);
    emit chatUpdate(this);
}

/**
 * Response signal for leaveChat.
 */
void ChatWidget::leaveChatCompleted(bool success, const QString &msg)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (!success) {
        QMessageBox::warning(this, "Leave Chat Failed",
                             "The server couldn't close the chat.\n" + msg);
    }
    emit leftChat();
    this->close();
}

/**
 * Response signal for invite.
 */
void ChatWidget::inviteCompleted(bool success, const QString &msg)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (!success) {
        QMessageBox::warning(this, "Invite Failed",
                             "The server couldn't invite the user you requested into the chat.\n" + msg);
    }
}

/**
  * Invites a user. The Format
  */
void ChatWidget::invite(QSharedPointer<const User> user, const QObject *id, bool success, const QString &msg)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (id != this)
        return;
    if (success) {
        invite(user);
    } else {
        QMessageBox::warning(this, "Invite Failed", msg);
    }
}

void ChatWidget::invite(QSharedPointer<const User> user)
{
    d_chat->invite(user);
}

void ChatWidget::leaveChat()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    d_chat->leaveChat();
}

/**
 * Response signal for send.
 */
void ChatWidget::sendCompleted(bool success, const QString &msg)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (!success) {
        QMessageBox::warning(this, "Send Failed",
                             "The message did not reach the other participants." + msg);
    }
}

ChatWidget::~ChatWidget()
{
    delete ui;
    delete pList;
}

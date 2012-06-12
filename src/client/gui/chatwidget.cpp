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
    qRegisterMetaType<QObject *>("QObject *");

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
    connect(d_chat.data(), SIGNAL(userJoined(QSharedPointer<const User>)),
            this, SLOT(userJoined(QSharedPointer<const User>)));
    connect(d_chat.data(), SIGNAL(userLeft(QSharedPointer<const User>)),
            this, SLOT(userLeft(QSharedPointer<const User>)));
    connect(d_chat.data(), SIGNAL(leaveChatCompleted(bool, QString)),
            this, SLOT(leaveChatCompleted(bool, QString)));
    connect(d_chat.data(), SIGNAL(inviteCompleted(bool, QString)),
            this, SLOT(inviteCompleted(bool, QString)));
    connect(d_chat.data(), SIGNAL(sendCompleted(bool, QString)),
            this, SLOT(sendCompleted(bool, QString)));
    connect(d_session.data(), SIGNAL(retrieveUserCompleted(QSharedPointer<const User>,
                                                           const QObject *, bool, QString)),
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

void ChatWidget::statusMessage(QSharedPointer<const User> user, const QString &message)
{
    QMutexLocker locker(&chatFontMutex);
    ui->tbChat->setFontWeight(QFont::Bold);
    ui->tbChat->append(QString("%1 %2").arg(user->getName(), message));
    ui->tbChat->setFontWeight(QFont::Normal);
}

/**
 * Another user joined the chat.
 */
void ChatWidget::userJoined(QSharedPointer<const User> user)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    pList->addUser(user);
    statusMessage(user, "joined");
    emit chatUpdate(this);
}

void ChatWidget::userLeft(QSharedPointer<const User> user)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    pList->removeUser(user);
    statusMessage(user, "left");
    emit chatUpdate(this);
}

/**
 * Response signal for leaveChat.
 */
void ChatWidget::leaveChatCompleted(bool, const QString &)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
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
                             "The server couldn't invite the user you "
                             "requested into the chat.\n" + msg);
    }
}

/**
  * Invites a user. The Format
  */
void ChatWidget::invite(QSharedPointer<const User> user, const QObject *id, bool success,
                        const QString &msg)
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

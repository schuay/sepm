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


#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QAbstractListModel>
#include <QMap>
#include <QMutex>
#include "chat.h"
#include "session.h"
#include "usermodel.h"

using namespace sdcc;

namespace Ui
{
class ChatWidget;
}

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QSharedPointer<Session> session, QSharedPointer<Chat> chat,
                        QWidget *parent = 0);
    /**
      * Dummy constructor, to create disabled window.
      */
    explicit ChatWidget(QWidget *parent = 0);
    ~ChatWidget();

signals:
    void leftChat();
    void chatUpdate(ChatWidget *changed);

public slots:
    // void chatClosed();
    /**
     * Another user sent a message to the chat.
     */
    void messageReceived(QSharedPointer<const User> user, const QString &msg);

    /**
     * Another user joined the chat.
     */
    void userJoined(QSharedPointer<const User> user);

    /**
     * A user left the chat.
     */
    void userLeft(QSharedPointer<const User> user);

    /**
     * Response signal for leaveChat.
     */
    void leaveChatCompleted(bool success, const QString &msg);

    /**
     * Response signal for invite.
     */
    void inviteCompleted(bool success, const QString &msg);

    /**
     * Response signal for send.
     */
    void sendCompleted(bool success, const QString &msg);

    /**
      * Invites a user. The Format
      */
    void invite(QSharedPointer<const User> user, const QObject *id, bool success,
                const QString &msg);
    void invite(QSharedPointer<const User> user);

    void leaveChat();


private slots:
    void returnPressed();

private:
    void statusMessage(QSharedPointer<const User> user, const QString &message);

    /**
     * Appends message to the chat window with the given font weight.
     * This doesn't affect selections.
     */
    void appendMessageWithWeight(const QString &message, int weight);

    Ui::ChatWidget *ui;
    QSharedPointer<Chat> d_chat;
    QSharedPointer<Session> d_session;
    UserModel *pList;
    QMutex chatFontMutex;
};

#endif // CHATWIDGET_H

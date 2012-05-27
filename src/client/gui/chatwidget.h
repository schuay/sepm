#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QAbstractListModel>
#include <QMap>
#include "chat.h"
#include "session.h"

using namespace sdcc;

namespace Ui
{
class ChatWidget;
}

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QSharedPointer<Session> session, QSharedPointer<Chat> chat, QWidget *parent = 0);
    /**
      * Dummy constructor, to create disabled window.
      */
    explicit ChatWidget(QWidget *parent = 0);
    ~ChatWidget();

signals:
    void leftChat();
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
    void invite(QSharedPointer<const User> user, const QObject *id, bool success, const QString &msg);
    void invite(QSharedPointer<const User> user);

    void leaveChat();


private slots:
    void returnPressed();

private:
    Ui::ChatWidget *ui;
    QSharedPointer<Chat> d_chat;
    QString lastMsg;
    QSharedPointer<Session> d_session;
};

#endif // CHATWIDGET_H

#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
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

public slots:
   // void chatClosed();
   /**
    * Another user sent a message to the chat.
    */
   void messageReceived(const User &user, const QString &msg);

   /**
    * Another user joined the chat.
    */
   void userJoined(const User &user);
   // /**
   //  * A user left the chat.
   //  */
   // void userLeft(const User &user);
//
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
   void invite(QSharedPointer<User> user);

private slots:
    void returnPressed();

private:
    Ui::ChatWidget *ui;
    QSharedPointer<Chat> d_chat;
    QString lastMsg;
    QSharedPointer<Session> session;
};

#endif // CHATWIDGET_H

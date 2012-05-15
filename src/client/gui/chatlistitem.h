#ifndef CHATLISTITEM_H
#define CHATLISTITEM_H

#include <QListWidgetItem>
#include "chatwidget.h"

class ChatListItem : public QObject, public QListWidgetItem
{
    Q_OBJECT
public:
    ChatListItem(ChatWidget *widget, QListWidget *view = 0);
    ChatWidget *getWidget();
private:
    ChatWidget *d_widget;
public slots:
    void onChatClosed();
};

#endif // CHATLISTITEM_H

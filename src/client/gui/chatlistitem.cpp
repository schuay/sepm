#include "chatlistitem.h"

ChatListItem::ChatListItem(ChatWidget *widget, QListWidget *view) : QObject(), QListWidgetItem(view)
{
    d_widget = widget;
    setText("A Chat");
    connect(widget, SIGNAL(destroyed()),
            this, SLOT(onChatClosed()));
}

ChatWidget *ChatListItem::getWidget()
{
    return d_widget;
}

void ChatListItem::onChatClosed()
{
    delete this;
}

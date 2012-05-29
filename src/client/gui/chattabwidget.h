#ifndef _CHATTABWIDGET_H
#define _CHATTABWIDGET_H

#include <QtGui>
#include "chatwidget.h"

using namespace sdcc;

namespace Ui
{
class ChatTabWidget;
}

class ChatTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    ChatTabWidget(QWidget *parent = 0);
    ~ChatTabWidget();

public slots:
    void onChatUpdate(ChatWidget *changed);
    void onCurrentChanged(int index);
};

#endif /* _CHATTABWIDGET_H */

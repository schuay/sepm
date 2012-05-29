#include "chattabwidget.h"
#include <QsLog.h>

ChatTabWidget::ChatTabWidget(QWidget *parent) : QTabWidget(parent)
{
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
}

ChatTabWidget::~ChatTabWidget()
{
}

void ChatTabWidget::onChatUpdate(ChatWidget *changed)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    int index = indexOf(changed);
    if (index == -1 || index == currentIndex()) {
        return;
    }

    tabBar()->setTabTextColor(index, Qt::red);
}

void ChatTabWidget::onCurrentChanged(int index)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (index == -1) {
        return;
    }

    tabBar()->setTabTextColor(index, Qt::black);
}

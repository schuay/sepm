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

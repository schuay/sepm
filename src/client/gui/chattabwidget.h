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

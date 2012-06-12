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


#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <session.h>
#include <QMainWindow>

#include "chat.h"
#include "chatwidget.h"
#include "usermodel.h"
#include <QtGui>

using namespace sdcc;

namespace Ui
{
class AppWindow;
}

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AppWindow(QWidget *parent = 0, QSharedPointer<Session> session
                       = QSharedPointer<Session>());
    ~AppWindow();

public slots:
    void logout();
    void onChatOpened(QSharedPointer<Chat> chat);
    void onChatOpened(QSharedPointer<Chat> chat, bool success, const QString &msg);
    void onSettingsButtonClicked();
    void onAddContactEntryClicked();
    void onSettingsEntryClicked();
    void onViewChatLogsEntryClicked();
    void onTabCloseRequested(int tab);
    void onStartChatEntryClicked();
    void onInviteClicked();
    void deleteAccount();
    void onLogoutCompleted(bool success = true, const QString &msg = "");
    void onUserDeleted(bool success, const QString &msg);
    void onContactListReceived(const QStringList &list, bool success, const QString &msg);
    void onLogoutReady();
    void contextMenuRequested(const QPoint &point);
    void onContactInvite(const QModelIndex &index);
private:
    void closeEvent(QCloseEvent *event);

    Ui::AppWindow *ui;
    QSharedPointer<Session> d_session;
    QMenu *settingspopupmenu;
    QMenu *contactListMenu;
    UserModel *contactList;
    QTimer *timer;
};

#endif // APPWINDOW_H

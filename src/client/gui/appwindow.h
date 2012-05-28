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
    void onTabCloseRequested(int tab);
    void onStartChatEntryClicked();
    void onInviteClicked();
    void deleteAccount();
    void onLogoutCompleted(bool success, const QString &msg);
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
};

#endif // APPWINDOW_H

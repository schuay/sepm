#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <session.h>
#include <QMainWindow>

#include "chat.h"
#include "chatwidget.h"
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
    void onLogoutClicked();
    void onChatOpened(QSharedPointer<Chat> chat);
    void onChatOpened(QSharedPointer<Chat> chat, bool success, const QString &msg);
    void onSettingsButtonClicked();
    void onAddContactEntryClicked();
    void onSettingsEntryClicked();
    void onTabCloseRequested(int tab);
    void onStartChatEntryClicked();
    void onInviteClicked();
    void onAddUserReturn(QSharedPointer<const User> user, const QObject *id, bool success, const QString &msg);
    void deleteAccount();
    void onLogoutCompleted(bool success, const QString &msg);
    void onUserDeleted(bool success, const QString &msg);
private:
    Ui::AppWindow *ui;
    QSharedPointer<Session> d_session;
    ChatWidget *activeChat;
    QMenu *settingspopupmenu;
    QList<QSharedPointer<const User> > inviteQueue;
};

#endif // APPWINDOW_H

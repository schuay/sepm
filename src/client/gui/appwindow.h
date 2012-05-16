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
    void onLogoutComplete(bool success, const QString &msg);
    void onChatOpened(QSharedPointer<Chat> chat);
    void onChatOpened(QSharedPointer<Chat> chat, bool success, const QString &msg);
    void onInitiateChatClicked();
    void onInviteClicked();
    void onChatActivated(ChatWidget *widget);
    void onActiveItemChanged(QListWidgetItem *widget);
    void onSettingsButtonClicked();
    void onAddContactEntryClicked();
    void onSettingsEntryClicked();
private:
    Ui::AppWindow *ui;
    QSharedPointer<Session> d_session;
    ChatWidget *activeChat;
    QMenu *settingspopupmenu;
};

#endif // APPWINDOW_H

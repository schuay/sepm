#include "appwindow.h"
#include "ui_appwindow.h"

#include "logindialog.h"
#include "chat.h"
#include "chatwidget.h"
#include "chatlistitem.h"
#include "settingsdialog.h"
#include <QListWidgetItem>
#include <QtGui>

Q_DECLARE_METATYPE(QSharedPointer<Chat>)

AppWindow::AppWindow(QWidget *parent, QSharedPointer<Session> session) :
    QMainWindow(parent),
    ui(new Ui::AppWindow)
{
    qRegisterMetaType<QSharedPointer<Chat> >("QSharedPointer<Chat>");
    d_session = session;
    ui->setupUi(this);

    connect(ui->pbLogout, SIGNAL(clicked()), this, SLOT(onLogoutClicked()));
    connect(ui->pbInitiateChat, SIGNAL(clicked()), this, SLOT(onInitiateChatClicked()));
    connect(ui->pbInvite, SIGNAL(clicked()), this, SLOT(onInviteClicked()));
    connect(ui->lwChats, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(onActiveItemChanged(QListWidgetItem*)));
    connect(ui->lwChats, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(onActiveItemChanged(QListWidgetItem*)));
    connect(ui->pbOptions, SIGNAL(clicked()),
            this, SLOT(onSettingsButtonClicked()));
    connect(this, SIGNAL(destroyed()), this, SLOT(onLogoutClicked()));
    connect(d_session.data(),
            SIGNAL(logoutCompleted(bool, QString)),
            this,
            SLOT(onLogoutComplete(bool,QString)));
    connect(d_session.data(),
            SIGNAL(initChatCompleted(QSharedPointer<Chat>, bool, QString)),
            this,
            SLOT(onChatOpened(QSharedPointer<Chat>,bool,QString)));
    connect(d_session.data(),
            SIGNAL(invitationReceived(QSharedPointer<Chat>)),
            this,
            SLOT(onChatOpened(QSharedPointer<Chat>)));
}

AppWindow::~AppWindow()
{
    delete ui;
}

void AppWindow::onLogoutClicked()
{
    d_session->logout();
}

void AppWindow::onLogoutComplete(bool success, const QString &msg)
{
    if(success) {
        close();
        delete this;
    } else {
        QMessageBox::warning(this, "Logout failed", msg);
    }
}

void AppWindow::onChatOpened(QSharedPointer<Chat> chat)
{
    ChatWidget *widget = new ChatWidget(d_session, chat);
    ui->swChats->addWidget(widget);
    ui->swChats->setCurrentWidget(widget);
    new ChatListItem(widget, ui->lwChats);
}

void AppWindow::onChatOpened(QSharedPointer<Chat> chat, bool success, const QString &msg)
{
    if(success) {
        onChatOpened(chat);
    } else {
        QMessageBox::warning(this, "Chat creation failed", msg);
    }
}

void AppWindow::onInitiateChatClicked()
{
    d_session->initChat();
}

void AppWindow::onInviteClicked()
{
    QString username = ui->leInvite->text();
    if(username.isEmpty()) {
        QMessageBox::warning(this, "Invite failed", "You didn't specify a username.");
    } else {
        QMessageBox::information(this, "Function not implemented", "Function is not yet implemented because it cannot be achieved with the current backend.");
    }
}
void AppWindow::onChatActivated(ChatWidget *widget)
{
    ui->swChats->setCurrentWidget(widget);
}

void AppWindow::onActiveItemChanged(QListWidgetItem *widget)
{
    ChatListItem *item = dynamic_cast<ChatListItem*>(widget);
    onChatActivated(item->getWidget());
}

void AppWindow::onSettingsButtonClicked()
{
    SettingsDialog *dialog = new SettingsDialog();
    dialog->show();
}

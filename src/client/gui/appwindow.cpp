#include "appwindow.h"
#include "ui_appwindow.h"

#include "logindialog.h"
#include "chat.h"
#include "chatwidget.h"
#include "settingsdialog.h"
#include <QListWidgetItem>
#include <QtGui>

Q_DECLARE_METATYPE(QSharedPointer<Chat>)

AppWindow::AppWindow(QWidget *parent, QSharedPointer<Session> session) :
    QMainWindow(parent),
    ui(new Ui::AppWindow)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    qRegisterMetaType<QSharedPointer<Chat> >("QSharedPointer<Chat>");
    qRegisterMetaType<QSharedPointer<const User> >("QSharedPointer<const User>");
    d_session = session;
    ui->setupUi(this);
    setWindowTitle("SDCC");
    //  connect(ui->pbInitiateChat, SIGNAL(clicked()), this, SLOT(onInitiateChatClicked()));
    //  connect(ui->pbInvite, SIGNAL(clicked()), this, SLOT(onInviteClicked()));
    connect(ui->twChats, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
    connect(ui->pbOptions, SIGNAL(clicked()),
            this, SLOT(onSettingsButtonClicked()));
    connect(d_session.data(),
            SIGNAL(logoutCompleted(bool, QString)),
            this,
            SLOT(onLogoutComplete(bool,QString)));
    connect(d_session.data(),
            SIGNAL(deleteUserCompleted(bool,QString)),
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
    connect(d_session.data(),
            SIGNAL(retrieveUserCompleted(QSharedPointer<const User>, const QObject*, bool, QString)),
            this,
            SLOT(onAddUserReturn(QSharedPointer<const User>,const QObject*,bool,QString)));
    connect(d_session.data(),
            SIGNAL(retrieveUserCompleted(QSharedPointer<const User>, const QObject*, bool, QString)),
            this,
            SLOT(onInviteUserReturn(QSharedPointer<const User>,const QObject*,bool,QString)));

    settingspopupmenu = new QMenu(this);
    settingspopupmenu->addAction("Add Contact",this,SLOT(onAddContactEntryClicked()));
    settingspopupmenu->addAction("Start Chat",this,SLOT(onStartChatEntryClicked()));
    settingspopupmenu->addAction("Options",this,SLOT(onSettingsEntryClicked()));
    settingspopupmenu->addAction("Logout",this,SLOT(onLogoutClicked()));
    ui->pbOptions->setMenu(settingspopupmenu);

    QRect rect = QApplication::desktop()->availableGeometry();
    this->move(rect.center() - this->rect().center());

}

AppWindow::~AppWindow()
{
    delete ui;
    delete settingspopupmenu;
}

void AppWindow::onAddContactEntryClicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Add Contact"),
                                         tr("User name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        d_session->retrieveUser(text, ui->lwChats);
    }
}

void AppWindow::onStartChatEntryClicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Start Chat with"),
                                         tr("User name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        d_session->retrieveUser(text, this);
    }

}

void AppWindow::onInviteUserReturn(QSharedPointer<const User> user, const QObject *id, bool success, const QString &msg)
{
    if(id != this)
        return;
    if(success) {
        inviteQueue.append(user);
        d_session->initChat();
    } else {
        QMessageBox::warning(this, "Couldn't find user", msg);
    }
}

void AppWindow::onLogoutClicked()
{
    d_session->logout();
}

void AppWindow::onLogoutComplete(bool success, const QString &msg)
{
    if(success) {
        LoginDialog *ld = new LoginDialog();
        ld->show();
        close();
    } else {
        QMessageBox::warning(this, "Logout failed", msg);
    }
}

void AppWindow::onChatOpened(QSharedPointer<Chat> chat)
{
    ChatWidget *widget = new ChatWidget(d_session, chat);
    ui->twChats->addTab(widget,chat->getID());
    ui->twChats->setCurrentWidget(widget);
}

void AppWindow::onChatOpened(QSharedPointer<Chat> chat, bool success, const QString &msg)
{
    if(success) {
        onChatOpened(chat);
        if(!inviteQueue.isEmpty()) {
            chat->invite(inviteQueue.first());
            inviteQueue.removeFirst();
        }
    } else {
        QMessageBox::warning(this, "Chat creation failed", msg);
    }
}

void AppWindow::onSettingsEntryClicked()
{
    SettingsDialog *dialog = new SettingsDialog(this,d_session);
    dialog->show();
}

void AppWindow::onSettingsButtonClicked()
{
    ui->pbOptions->menu()->exec(QCursor::pos());
}

void AppWindow::onTabCloseRequested(int tab)
{
    ui->twChats->removeTab(tab);
}

void AppWindow::onAddUserReturn(QSharedPointer<const User> user, const QObject *id, bool success, const QString &msg)
{
    if(id != ui->lwChats)
        return;
    if(success) {
        user->getName();
        QMessageBox::information(this, "Contact list not implemented yet", "You can't add contacts here yet. Sorry.");
    } else {
        QMessageBox::warning(this, "Couldn't find user", msg);
    }
}

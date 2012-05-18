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
    qRegisterMetaType<QSharedPointer<Chat> >("QSharedPointer<Chat>");
    d_session = session;
    ui->setupUi(this);
    setWindowTitle("SDCC");
  //  connect(ui->pbInitiateChat, SIGNAL(clicked()), this, SLOT(onInitiateChatClicked()));
  //  connect(ui->pbInvite, SIGNAL(clicked()), this, SLOT(onInviteClicked()));
    connect(ui->twChats, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
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
        if (ok && !text.isEmpty()){

        }
}

void AppWindow::onStartChatEntryClicked(){
    bool ok;
        QString text = QInputDialog::getText(this, tr("Start Chat with"),
                                             tr("User name:"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty()){

        }

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
    ui->twChats->addTab(widget,chat->getID());
    ui->twChats->setCurrentWidget(widget);
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

/*
void AppWindow::onInviteClicked()
{
    QString username = ui->leInvite->text();
    if(username.isEmpty()) {
        QMessageBox::warning(this, "Invite failed", "You didn't specify a username.");
    } else {
        QMessageBox::information(this, "Function not implemented",
                                 "Function is not yet implemented because it cannot be achieved "
                                 "with the current backend.");
    }
}*/

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

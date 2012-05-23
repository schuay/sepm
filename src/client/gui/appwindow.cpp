#include "appwindow.h"
#include "ui_appwindow.h"

#include "logindialog.h"
#include "chat.h"
#include "chatwidget.h"
#include "settingsdialog.h"
#include <QListWidgetItem>
#include <QtGui>
#include <QsLog.h>

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
    connect(ui->twChats, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
    connect(ui->pbOptions, SIGNAL(clicked()),
            this, SLOT(onSettingsButtonClicked()));
    connect(d_session.data(),
            SIGNAL(deleteUserCompleted(bool, QString)),
            this,
            SLOT(onUserDeleted(bool, QString)));
    connect(d_session.data(),
            SIGNAL(initChatCompleted(QSharedPointer<Chat>, bool, QString)),
            this,
            SLOT(onChatOpened(QSharedPointer<Chat>, bool, QString)));
    connect(d_session.data(),
            SIGNAL(invitationReceived(QSharedPointer<Chat>)),
            this,
            SLOT(onChatOpened(QSharedPointer<Chat>)));
    connect(d_session.data(),
            SIGNAL(retrieveUserCompleted(QSharedPointer<const User>, const QObject *, bool, QString)),
            this,
            SLOT(onAddUserReturn(QSharedPointer<const User>, const QObject *, bool, QString)));

    settingspopupmenu = new QMenu(this);
    settingspopupmenu->addAction("Add Contact", this, SLOT(onAddContactEntryClicked()));
    settingspopupmenu->addAction("Start Chat", this, SLOT(onStartChatEntryClicked()));
    settingspopupmenu->addAction("Invite", this, SLOT(onInviteClicked()));
    settingspopupmenu->addAction("Options", this, SLOT(onSettingsEntryClicked()));
    settingspopupmenu->addAction("Logout", this, SLOT(onLogoutClicked()));
    ui->pbOptions->setMenu(settingspopupmenu);

    QRect rect = QApplication::desktop()->availableGeometry();
    this->move(rect.center() - this->rect().center());

}

AppWindow::~AppWindow()
{
    if(!d_session->isValid())
        d_session->logout();
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
        d_session->retrieveUser(text, this);
    }
}

void AppWindow::onInviteClicked()
{
    if (ui->twChats->count() == 0) {
        QMessageBox::warning(this, "No chat opened", "You can't invite a user without first opening up a chat");
        return;
    }
    bool ok;
    QString text = QInputDialog::getText(this, tr("Start Chat with"),
                                         tr("User name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        d_session->retrieveUser(text, ui->twChats->currentWidget());
    }

}

void AppWindow::onLogoutClicked()
{
    close();
}

void AppWindow::onChatOpened(QSharedPointer<Chat> chat)
{
    ChatWidget *widget = new ChatWidget(d_session, chat);
    ui->twChats->addTab(widget, chat->getID());
    ui->twChats->setCurrentWidget(widget);
}

void AppWindow::onChatOpened(QSharedPointer<Chat> chat, bool success, const QString &msg)
{
    if (success) {
        onChatOpened(chat);
        if (!inviteQueue.isEmpty()) {
            chat->invite(inviteQueue.first());
            inviteQueue.removeFirst();
        }
    } else {
        QMessageBox::warning(this, "Chat creation failed", msg);
    }
}

void AppWindow::onSettingsEntryClicked()
{
    SettingsDialog *dialog = new SettingsDialog(this);
    dialog->show();
}

void AppWindow::onSettingsButtonClicked()
{
    ui->pbOptions->menu()->exec(QCursor::pos());
}

void AppWindow::onTabCloseRequested(int tab)
{
    ChatWidget *cw = dynamic_cast<ChatWidget*>(ui->twChats->widget(tab));
    cw->leaveChat();
    ui->twChats->removeTab(tab);
}

void AppWindow::onAddUserReturn(QSharedPointer<const User> user, const QObject *id, bool success, const QString &msg)
{
    if (id != this)
        return;
    if (success) {
        user->getName();
        QMessageBox::information(this, "Contact list not implemented yet", "You can't add contacts here yet. Sorry.");
    } else {
        QMessageBox::warning(this, "Couldn't find user", msg);
    }
}

void AppWindow::deleteAccount() {
    d_session->deleteUser(d_session->getUser());
}

void AppWindow::onUserDeleted(bool success, const QString &msg)
{
    if(success) {
        close();
    } else {
        QMessageBox::warning(this, "User couldn't be deleted", msg);
    }
}

void AppWindow::onStartChatEntryClicked() {
    d_session->initChat();
}

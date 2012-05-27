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
    ui->setupUi(this);
    d_session = session;
    contactList = new UserModel(d_session);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->lUsername->setText(d_session->getUser()->getName());

    setWindowTitle("SDCC");

    qRegisterMetaType<QSharedPointer<Chat> >("QSharedPointer<Chat>");
    qRegisterMetaType<QSharedPointer<const User> >("QSharedPointer<const User>");

    connect(ui->twChats, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
    connect(ui->pbOptions, SIGNAL(clicked()),
            this, SLOT(onSettingsButtonClicked()));
    connect(d_session.data(),
            SIGNAL(deleteUserCompleted(bool, QString)),
            this,
            SLOT(onUserDeleted(bool, QString)));
    connect(d_session.data(),
            SIGNAL(logoutCompleted(bool, QString)),
            this,
            SLOT(onLogoutCompleted(bool, QString)));
    connect(d_session.data(),
            SIGNAL(initChatCompleted(QSharedPointer<Chat>, bool, QString)),
            this,
            SLOT(onChatOpened(QSharedPointer<Chat>, bool, QString)));
    connect(d_session.data(),
            SIGNAL(invitationReceived(QSharedPointer<Chat>)),
            this,
            SLOT(onChatOpened(QSharedPointer<Chat>)));
    connect(d_session.data(),
            SIGNAL(retrieveContactListCompleted(const QStringList, bool, QString)),
            this,
            SLOT(onContactListReceived(const QStringList, bool, QString)));
    connect(d_session.data(),
            SIGNAL(saveContactListCompleted(bool, QString)),
            this,
            SLOT(onLogoutReady()));
    connect(ui->lvContacts,
            SIGNAL(customContextMenuRequested(QPoint)),
            this,
            SLOT(contextMenuRequested(QPoint)));
    connect(ui->lvContacts,
            SIGNAL(activated(QModelIndex)),
            this,
            SLOT(onContactInvite(QModelIndex)));

    ui->lvContacts->setModel(contactList);
    d_session->retrieveContactList();
    settingspopupmenu = new QMenu(this);
    settingspopupmenu->addAction("Add Contact", this, SLOT(onAddContactEntryClicked()));
    settingspopupmenu->addAction("Start Chat", this, SLOT(onStartChatEntryClicked()));
    settingspopupmenu->addAction("Invite", this, SLOT(onInviteClicked()));
    settingspopupmenu->addAction("Options", this, SLOT(onSettingsEntryClicked()));
    settingspopupmenu->addAction("Logout", this, SLOT(logout()));
    ui->pbOptions->setMenu(settingspopupmenu);

    contactListMenu = new QMenu(this);
    contactListMenu->addAction("Delete");
    contactListMenu->addAction("Invite");

    ui->lvContacts->setContextMenuPolicy(Qt::CustomContextMenu);

    QRect rect = QApplication::desktop()->availableGeometry();
    this->move(rect.center() - this->rect().center());
}

AppWindow::~AppWindow()
{
    if (d_session->isValid())
        d_session->logout();
    delete ui;
    delete settingspopupmenu;
    delete contactList;
}

void AppWindow::closeEvent(QCloseEvent *event)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (d_session->isValid()) {
        logout();
        event->ignore();
    }
}

void AppWindow::onAddContactEntryClicked()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    bool ok;
    QString text = QInputDialog::getText(this, tr("Add Contact"),
                                         tr("User name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        d_session->retrieveUser(text, contactList);
    }
}

void AppWindow::onInviteClicked()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (ui->twChats->count() == 0) {
        QMessageBox::warning(this, "Invite Failed", "To invite a user, please create or join a chat.");
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

void AppWindow::logout()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (d_session->isValid()) {
        d_session->saveContactList(contactList->toUserList());
    }
}

void AppWindow::onChatOpened(QSharedPointer<Chat> chat)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    ChatWidget *widget = new ChatWidget(d_session, chat);
    ui->twChats->addTab(widget, chat->getID());
    ui->twChats->setCurrentWidget(widget);
}

void AppWindow::onChatOpened(QSharedPointer<Chat> chat, bool success, const QString &msg)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (success) {
        onChatOpened(chat);
        if (!inviteQueue.isEmpty()) {
            chat->invite(inviteQueue.first());
            inviteQueue.removeFirst();
        }
    } else {
        QMessageBox::warning(this, "Chat Creation Failed", msg);
    }
}

void AppWindow::onSettingsEntryClicked()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    SettingsDialog *dialog = new SettingsDialog(this);
    dialog->show();
}

void AppWindow::onSettingsButtonClicked()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    ui->pbOptions->menu()->exec(QCursor::pos());
}

void AppWindow::onTabCloseRequested(int tab)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    ChatWidget *cw = dynamic_cast<ChatWidget *>(ui->twChats->widget(tab));
    cw->leaveChat();
    ui->twChats->removeTab(tab);
}

void AppWindow::deleteAccount()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    d_session->deleteUser(d_session->getUser());
}

void AppWindow::onLogoutCompleted(bool success, const QString &msg)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (success) {
        LoginDialog *ld = new LoginDialog();
        ld->show();
        close();
    } else {
        QMessageBox::warning(this, "Logout Failed", msg);
    }
}

void AppWindow::onUserDeleted(bool success, const QString &msg)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (success) {
        LoginDialog *ld = new LoginDialog();
        ld->show();
        close();
    } else {
        QMessageBox::warning(this, "User Deletion Failed", msg);
    }
}

void AppWindow::onContactListReceived(const QStringList &list, bool success, const QString &msg)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (success) {
        QLOG_DEBUG() << list;
        contactList->fromNameList(list);
    } else
        QMessageBox::warning(this, "Contact list couldn't be loaded", msg);
}

void AppWindow::onLogoutReady()
{
    if (d_session->isValid()) {
        d_session->logout();
    }
}

void AppWindow::contextMenuRequested(const QPoint &point)
{
    QPoint mousePoint = ui->lvContacts->mapToGlobal(point);
    QModelIndex index = ui->lvContacts->indexAt(point);
    QAction *action = contactListMenu->exec(mousePoint);
    if (action == 0) return;
    if (action->text() == "Delete") {
        contactList->removeUser(index);
    }
    if (action->text() == "Invite") {
        onContactInvite(index);
    }
}

void AppWindow::onContactInvite(const QModelIndex &index)
{
    ChatWidget *cw = dynamic_cast<ChatWidget *>(ui->twChats->currentWidget());
    if (cw == 0) return;
    cw->invite(contactList->getUser(index));
}

void AppWindow::onStartChatEntryClicked()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    d_session->initChat();
}

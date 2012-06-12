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


#include "appwindow.h"
#include "ui_appwindow.h"

#include "logindialog.h"
#include "chat.h"
#include "chatwidget.h"
#include "settingsdialog.h"
#include "chatlogdialog.h"
#include <QListWidgetItem>
#include <QtGui>
#include <QsLog.h>
#define TIMEOUT 5

Q_DECLARE_METATYPE(QSharedPointer<Chat>)

AppWindow::AppWindow(QWidget *parent, QSharedPointer<Session> session) :
    QMainWindow(parent),
    ui(new Ui::AppWindow)
{
    ui->setupUi(this);
    d_session = session;
    contactList = new UserModel(d_session);
    timer = new QTimer(this);
    timer->setInterval(TIMEOUT * 1000);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->lUsername->setText(d_session->getUser()->getName());

    setWindowIcon(QIcon(":/appicon.png"));

    qRegisterMetaType<QSharedPointer<Chat> >("QSharedPointer<Chat>");
    qRegisterMetaType<QSharedPointer<const User> >("QSharedPointer<const User>");

    connect(ui->twChats, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
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
    connect(timer,
            SIGNAL(timeout()),
            this,
            SLOT(onLogoutCompleted()));

    /* Setup tool buttons. */
    ui->tbAddContact->setIcon(QIcon(":/addcontact.png"));
    ui->tbStartChat->setIcon(QIcon(":/startchat.png"));
    ui->tbChatlogs->setIcon(QIcon(":/chatlogs.png"));
    ui->tbInvite->setIcon(QIcon(":/invite.png"));
    ui->tbOptions->setIcon(QIcon(":/options.png"));
    ui->tbLogout->setIcon(QIcon(":/logout.png"));

    connect(ui->tbAddContact, SIGNAL(clicked()), this, SLOT(onAddContactEntryClicked()));
    connect(ui->tbStartChat, SIGNAL(clicked()), this, SLOT(onStartChatEntryClicked()));
    connect(ui->tbChatlogs, SIGNAL(clicked()), this, SLOT(onViewChatLogsEntryClicked()));
    connect(ui->tbInvite, SIGNAL(clicked()), this, SLOT(onInviteClicked()));
    connect(ui->tbOptions, SIGNAL(clicked()), this, SLOT(onSettingsEntryClicked()));
    connect(ui->tbLogout, SIGNAL(clicked()), this, SLOT(logout()));

    ui->lvContacts->setModel(contactList);
    d_session->retrieveContactList();
    settingspopupmenu = new QMenu(this);
    settingspopupmenu->addAction("Add Contact", this, SLOT(onAddContactEntryClicked()));
    settingspopupmenu->addAction("Start Chat", this, SLOT(onStartChatEntryClicked()));
    settingspopupmenu->addAction("View Chatlogs", this, SLOT(onViewChatLogsEntryClicked()));
    settingspopupmenu->addAction("Invite", this, SLOT(onInviteClicked()));
    settingspopupmenu->addAction("Options", this, SLOT(onSettingsEntryClicked()));
    settingspopupmenu->addAction("Logout", this, SLOT(logout()));

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
        timer->start();
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
    connect(widget, SIGNAL(chatUpdate(ChatWidget *)), ui->twChats,
            SLOT(onChatUpdate(ChatWidget *)));
    ui->twChats->addTab(widget, chat->getID());
    ui->twChats->setCurrentWidget(widget);
}

void AppWindow::onChatOpened(QSharedPointer<Chat> chat, bool success, const QString &msg)
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (success) {
        onChatOpened(chat);
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
        if (timer->isActive())
            timer->stop();
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
    if (success)
        contactList->fromNameList(list);
    else
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
    if (!index.isValid()) return;
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
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    if (ui->twChats->count() == 0) {
        QMessageBox::warning(this, "Invite Failed", "To invite a user, please create or join a chat.");
        return;
    }
    ChatWidget *cw = dynamic_cast<ChatWidget *>(ui->twChats->currentWidget());
    if (cw == 0) return;
    cw->invite(contactList->getUser(index));
}

void AppWindow::onStartChatEntryClicked()
{
    QLOG_TRACE() << __PRETTY_FUNCTION__;
    d_session->initChat();
}

void AppWindow::onViewChatLogsEntryClicked()
{
    ChatLogDialog *dialog = new ChatLogDialog(this, d_session);
    dialog->show();
}

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


#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <Qt>


#include "sessionmanager.h"
#include "appwindow.h"
#include <iostream>

using namespace sdcc;

Q_DECLARE_METATYPE(QSharedPointer<Session>)

LoginDialog::LoginDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginDialog), busyWorkers(0)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);

    setWindowIcon(QIcon(":/appicon.png"));

    qRegisterMetaType<QSharedPointer<Session> >("QSharedPointer<Session>");

    connect(ui->pbTestConnection, SIGNAL(clicked()), this, SLOT(onTestConnectionClicked()));
    connect(sdcc::SessionManager::getInstance(), SIGNAL(testConnectionCompleted(bool, QString)),
            this, SLOT(onTestConnectionCompleted(bool, QString)));
    connect(ui->pbRegister, SIGNAL(clicked()), this, SLOT(onRegisterUserClicked()));
    connect(sdcc::SessionManager::getInstance(), SIGNAL(registerCompleted(bool, QString)),
            this, SLOT(onRegisterUserCompleted(bool, QString)));
    connect(ui->pbServerCert, SIGNAL(clicked()), this, SLOT(onChooseServerCertClicked()));
    connect(ui->pbPublicKey, SIGNAL(clicked()), this, SLOT(onChoosePublicKeyClicked()));
    connect(ui->pbPrivateKey, SIGNAL(clicked()), this, SLOT(onChoosePrivateKeyClicked()));
    connect(ui->pbConnect, SIGNAL(clicked()), this, SLOT(onConnectClicked()));
    connect(ui->groupBox_2, SIGNAL(toggled(bool)), this, SLOT(onGroupCheckboxClicked(bool)));

    connect(SessionManager::getInstance(),
            SIGNAL(loginCompleted(QSharedPointer<Session>, bool, QString)),
            this,
            SLOT(onConnectFinished(QSharedPointer<Session>, bool, QString)));

    progressBar = new QProgressBar(this);
    progressBar->setTextVisible(false);
    statusBar()->addPermanentWidget(progressBar, 0);

    ui->leServerCert->setText(this->settings.getValue(settings.CCertPath).toString());
    ui->lePublicKey->setText(this->settings.getValue(settings.CPublicKeyPath).toString());
    ui->lePrivateKey->setText(this->settings.getValue(settings.CPrivateKeyPath).toString());
    ui->leUsername->setText(this->settings.getValue(settings.CUsername).toString());
    ui->leServer->setText(this->settings.getValue(settings.CServer).toString());

    ui->groupBox_2->setCheckable(true);
    onGroupCheckboxClicked(ui->leUsername->text().isEmpty());

    QRect rect = QApplication::desktop()->availableGeometry();
    this->move(rect.center() - this->rect().center());
}

void LoginDialog::onGroupCheckboxClicked(bool checked)
{
    QList<QWidget *> ch = ui->groupBox_2->findChildren<QWidget *>();
    for (int i = 0; i < ch.count(); i++) ch[i]->setVisible(checked);
    ui->groupBox_2->setChecked(checked);
    ui->groupBox_2->setFlat(!checked);

    if (!checked) {
        ui->groupBox_2->setMaximumSize(60, 20);
        int oldSize = this->geometry().width();
        resize(oldSize, 100);
        adjustSize();
        resize(oldSize, 100);
    } else {
        ui->groupBox_2->setMaximumSize(16777215, 16777215);
        int oldSize = this->geometry().width();
        resize(oldSize, 480);
    }
}

void LoginDialog::onChooseServerCertClicked()
{
    QString filter = "Certificate Files (*.crt)";
    QString temp = QFileDialog::getOpenFileName(this, "Select Server Certificate",
                   ui->leServerCert->text(), filter, 0);
    if (!temp.isNull())
        ui->leServerCert->setText(temp);

}

void LoginDialog::onChoosePrivateKeyClicked()
{
    QString filter = "Key Files (*.pem)";
    QString temp = QFileDialog::getOpenFileName(this, "Select Private Key", ui->lePrivateKey->text(), filter, 0);
    if (!temp.isNull())
        ui->lePrivateKey->setText(temp);

}

void LoginDialog::onChoosePublicKeyClicked()
{
    QString filter = "Key Files (*.pem)";
    QString temp = QFileDialog::getOpenFileName(this, "Select Public Key", ui->lePublicKey->text(), filter, 0);
    if (!temp.isNull())
        ui->lePublicKey->setText(temp);
}

void LoginDialog::onTestConnectionClicked()
{
    if (ui->leServer->text().isEmpty()) {
        QMessageBox::warning(this, "Missing Arguments",
                             "Please make sure the server field is filled in.");
        return;
    }

    addWorker();
    ui->pbTestConnection->setEnabled(false);
    sdcc::SessionManager::testConnection(ui->leServer->text());
}

void LoginDialog::onRegisterUserClicked()
{
    if (ui->leServer->text().isEmpty()
            || ui->leUsername->text().isEmpty()
            || ui->lePublicKey->text().isEmpty()
            || ui->lePrivateKey->text().isEmpty()
            || ui->leServerCert->text().isEmpty()
            || ui->lePassword->text().isEmpty()) {
        QMessageBox::warning(this, "Missing Arguments",
                             QString("Please make sure the username, password, public key, private key, "
                                     "server and server certificate fields are filled in."));
        return;
    }

    try {
        QSharedPointer<const User> u(new User(ui->leUsername->text() + QString("@")
                                              + ui->leServer->text(), ui->lePublicKey->text()));

        addWorker();
        ui->pbRegister->setEnabled(false);
        sdcc::SessionManager::registerUser(ui->leServer->text(), ui->leServerCert->text(),
                                           u, ui->lePassword->text());
    } catch (const sdc::SecurityException &e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void LoginDialog::onConnectClicked()
{
    if (ui->leServer->text().isEmpty()
            || ui->leUsername->text().isEmpty()
            || ui->lePublicKey->text().isEmpty()
            || ui->lePrivateKey->text().isEmpty()
            || ui->leServerCert->text().isEmpty()
            || ui->lePassword->text().isEmpty()) {
        QMessageBox::warning(this, "Missing Arguments",
                             QString("Please make sure the username, password, public key, private key, "
                                     "server and server certificate fields are filled in."));
        return;
    }

    try {
        QSharedPointer<const LoginUser> u(new LoginUser(ui->leUsername->text() + QString("@")
                                          + ui->leServer->text(), ui->lePublicKey->text(),
                                          ui->lePrivateKey->text()));

        sdcc::SessionManager::login(ui->leServer->text(),
                                    ui->leServerCert->text(),
                                    u,
                                    ui->lePassword->text());
        addWorker();
        ui->pbConnect->setEnabled(false);
    } catch (const sdc::SecurityException &e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void LoginDialog::onConnectFinished(QSharedPointer<Session> session, bool success, const QString &msg)
{
    removeWorker();
    if (success) {
        AppWindow *win = new AppWindow(0, session);
        win->show();
        close();
    } else {
        QMessageBox::warning(this, "Login Failed", msg);
    }
    ui->pbConnect->setEnabled(true);
}

void LoginDialog::addWorker()
{
    if (busyWorkers == 0) {
        /* Setting maximum to 0 switches bar to 'busy' mode. */
        progressBar->setMaximum(0);
    }
    busyWorkers++;
}

void LoginDialog::removeWorker()
{
    if (--busyWorkers == 0) {
        progressBar->setMaximum(100);
        progressBar->reset();
    }
}

void LoginDialog::onRegisterUserCompleted(bool success, const QString &msg)
{
    removeWorker();
    ui->pbRegister->setEnabled(true);
    if (success) {
        statusBar()->showMessage("Success!", 5000);
    } else {
        QMessageBox::information(this, "User Registration Failed",
                                 QString("User registration failed: %1")
                                 .arg(msg));
    }

}

void LoginDialog::onTestConnectionCompleted(bool success, const QString &msg)
{
    removeWorker();
    ui->pbTestConnection->setEnabled(true);
    if (success) {
        statusBar()->showMessage("Success!", 5000);
    } else {
        QMessageBox::information(this, "Connection Test Failed",
                                 QString("Connection test failed: %1")
                                 .arg(msg));
    }
}


LoginDialog::~LoginDialog()
{
    settings.setValue(settings.CUsername, ui->leUsername->text());
    settings.setValue(settings.CServer, ui->leServer->text());
    settings.setValue(settings.CPublicKeyPath, ui->lePublicKey->text());
    settings.setValue(settings.CCertPath, ui->leServerCert->text());
    settings.setValue(settings.CPrivateKeyPath, ui->lePrivateKey->text());
    delete ui;
}

/* *************************************************************************
 *  Copyright 2012 Jakob Gruber                                            *
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

#include "sessionmanager.h"

using namespace sdcc;

LoginDialog::LoginDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginDialog), busyWorkers(0)
{
    ui->setupUi(this);

    connect(ui->pbTestConnection, SIGNAL(clicked()), this, SLOT(onTestConnectionClicked()));
    connect(sdcc::SessionManager::getInstance(), SIGNAL(testConnectionCompleted(bool,QString)),
            this, SLOT(onTestConnectionCompleted(bool,QString)));
    connect(ui->pbRegister, SIGNAL(clicked()), this, SLOT(onRegisterUserClicked()));
    connect(sdcc::SessionManager::getInstance(), SIGNAL(registerCompleted(bool,QString)),
            this, SLOT(onRegisterUserCompleted(bool,QString)));
    connect(ui->pbServerCert, SIGNAL(clicked()), this, SLOT(onChooseServerCertClicked()));
    connect(ui->pbPublicKey, SIGNAL(clicked()), this, SLOT(onChoosePublicKeyClicked()));
    connect(ui->pbPrivateKey, SIGNAL(clicked()), this, SLOT(onChoosePrivateKeyClicked()));

    progressBar = new QProgressBar(this);
    progressBar->setTextVisible(false);
    statusBar()->addPermanentWidget(progressBar, 0);
}

void LoginDialog::onChooseServerCertClicked()
{
    ui->leServerCert->setText(QFileDialog::getOpenFileName(this, "Select server certificate..."));
}

void LoginDialog::onChoosePrivateKeyClicked()
{
    ui->lePrivateKey->setText(QFileDialog::getOpenFileName(this, "Select private key..."));
}

void LoginDialog::onChoosePublicKeyClicked()
{
    ui->lePublicKey->setText(QFileDialog::getOpenFileName(this, "Select public key..."));
}

void LoginDialog::onTestConnectionClicked()
{
    if (ui->leServer->text().isEmpty())
        return;

    addWorker();
    ui->pbTestConnection->setEnabled(false);
    sdcc::SessionManager::testConnection(ui->leServer->text());
}

void LoginDialog::onRegisterUserClicked()
{
    if (ui->leServer->text().isEmpty()
            || ui->leUsername->text().isEmpty()
            || ui->lePublicKey->text().isEmpty()
            || ui->leServerCert->text().isEmpty()
            || ui->lePassword->text().isEmpty()) {
        return;
    }

    User u(ui->leUsername->text(), ui->lePublicKey->text());

    addWorker();
    ui->pbRegister->setEnabled(false);
    sdcc::SessionManager::registerUser(ui->leServer->text(), ui->leServerCert->text(),
                                       u, ui->lePassword->text());
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
    }
}

void LoginDialog::onRegisterUserCompleted(bool success, const QString &msg)
{
    removeWorker();
    ui->pbRegister->setEnabled(true);
    if (success) {
        statusBar()->showMessage("Success!", 5000);
    } else {
        QMessageBox::information(this, "User registration failed",
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
        QMessageBox::information(this, "Connection test failed",
                                 QString("Connection test failed: %1")
                                 .arg(msg));
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

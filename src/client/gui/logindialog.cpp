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

#include "sessionmanager.h"

using namespace sdcc;

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
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
    sdcc::SessionManager::testConnection(ui->leServer->text());
}

void LoginDialog::onRegisterUserClicked()
{
    User u(ui->leUsername->text(), ui->lePublicKey->text());
    sdcc::SessionManager::registerUser(ui->leServer->text(), ui->leServerCert->text(),
                                       u, "password");
}

void LoginDialog::onRegisterUserCompleted(bool success, const QString &msg)
{
    QMessageBox::information(this, "User registration completed",
                             QString("User registration %1 %2")
                             .arg(success ? "completed successfully." : "failed: ")
                             .arg(msg));

}

void LoginDialog::onTestConnectionCompleted(bool success, const QString &msg)
{
    QMessageBox::information(this, "Connection test completed",
                             QString("Connection test %1 %2")
                             .arg(success ? "completed successfully." : "failed: ")
                             .arg(msg));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

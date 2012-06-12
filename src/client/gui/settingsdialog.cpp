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


#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>
#include <QMessageBox>

#include "appwindow.h"

using namespace sdcc;

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle("Settings");
    connect(ui->pbPublicKey, SIGNAL(clicked()), this, SLOT(onChoosePublicKeyClicked()));
    connect(ui->pbPrivateKey, SIGNAL(clicked()), this, SLOT(onChoosePrivateKeyClicked()));
    connect(ui->okButton, SIGNAL(accepted()), this, SLOT(okButtonClicked()));
    connect(ui->okButton, SIGNAL(rejected()), this, SLOT(close()));
    connect(ui->pbDeleteAccount, SIGNAL(clicked()), this, SLOT(onDeleteAccountClicked()));
    connect(ui->pbServerCert, SIGNAL(clicked()), this, SLOT(onChooseServerCertClicked()));

    ui->lePublicKey->setText(this->settings.getValue(settings.CPublicKeyPath).toString());
    ui->lePrivateKey->setText(this->settings.getValue(settings.CPrivateKeyPath).toString());
    ui->leServerCert->setText(this->settings.getValue(settings.CCertPath).toString());


}

void SettingsDialog::okButtonClicked()
{
    settings.setValue(settings.CPrivateKeyPath, ui->lePrivateKey->text());
    settings.setValue(settings.CPublicKeyPath, ui->lePublicKey->text());
    settings.setValue(settings.CCertPath, ui->leServerCert->text());
    close();
}

void SettingsDialog::onChooseServerCertClicked()
{
    QString filter = "Certificate Files (*.crt)";
    QString temp = QFileDialog::getOpenFileName(this, "Select Server Certificate", ui->leServerCert->text(), filter, 0);
    if (!temp.isNull())
        ui->leServerCert->setText(temp);
}

void SettingsDialog::onChoosePrivateKeyClicked()
{
    QString filter = "Key Files (*.pem)";
    QString temp = QFileDialog::getOpenFileName(this, "Select Private Key", ui->lePrivateKey->text(), filter, 0);
    if (!temp.isNull())
        ui->lePrivateKey->setText(temp);
}

void SettingsDialog::onChoosePublicKeyClicked()
{
    QString filter = "Key Files (*.pem)";
    QString temp = QFileDialog::getOpenFileName(this, "Select Public Key", ui->lePublicKey->text(), filter, 0);
    if (!temp.isNull())
        ui->lePublicKey->setText(temp);
}

void SettingsDialog::onDeleteAccountClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Delete Account"),
                                  "Are you really sure?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        AppWindow *aw = dynamic_cast<AppWindow *>(parent());
        aw->deleteAccount();
    }

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

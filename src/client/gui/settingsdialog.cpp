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
    QString filter = "CertFiles (*.crt)";
    QString temp = QFileDialog::getOpenFileName(this, "Select server certificate...", ui->leServerCert->text(), filter, 0);
    if (!temp.isNull())
        ui->leServerCert->setText(temp);
    settings.setValue(settings.CCertPath, ui->leServerCert->text());
}

void SettingsDialog::onChoosePrivateKeyClicked()
{
    QString filter = "KeyFiles (*.pem)";
    QString temp = QFileDialog::getOpenFileName(this, "Select private key...", ui->lePrivateKey->text(), filter, 0);
    if (!temp.isNull())
        ui->lePrivateKey->setText(temp);
    settings.setValue(settings.CPrivateKeyPath, ui->lePrivateKey->text());
}

void SettingsDialog::onChoosePublicKeyClicked()
{
    QString filter = "KeyFiles (*.pem)";
    QString temp = QFileDialog::getOpenFileName(this, "Select public key...", ui->lePublicKey->text(), filter, 0);
    if (!temp.isNull())
        ui->lePublicKey->setText(temp);
    settings.setValue(settings.CPublicKeyPath, ui->lePublicKey->text());
}

void SettingsDialog::onDeleteAccountClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Delete Account"),
                                  "Are you really sure?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        AppWindow *aw = dynamic_cast<AppWindow*>(parent());
        aw->deleteAccount();
    }

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

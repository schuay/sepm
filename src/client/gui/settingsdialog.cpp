#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>
#include <QMessageBox>

using namespace sdcc;

SettingsDialog::SettingsDialog(QWidget *parent, QSharedPointer<Session> session) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    d_session = session;
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
    QString temp = QFileDialog::getOpenFileName(this, "Selectd_session->logout(); server certificate...", ui->leServerCert->text());
    if (!temp.isNull())
        ui->leServerCert->setText(temp);
    settings.setValue(settings.CCertPath, ui->leServerCert->text());
}

void SettingsDialog::onChoosePrivateKeyClicked()
{
    QString temp = QFileDialog::getOpenFileName(this, "Select private key...", ui->lePrivateKey->text());
    if (!temp.isNull())
        ui->lePrivateKey->setText(temp);
    settings.setValue(settings.CPrivateKeyPath, ui->lePrivateKey->text());
}

void SettingsDialog::onChoosePublicKeyClicked()
{
    QString temp = QFileDialog::getOpenFileName(this, "Select public key...", ui->lePublicKey->text());
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
        d_session->deleteUser(d_session->getUser());
    }

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

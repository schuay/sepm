#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>

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

    ui->lePublicKey->setText(this->settings.getValue(settings.CPublicKeyPath).toString());
    ui->lePrivateKey->setText(this->settings.getValue(settings.CPrivateKeyPath).toString());

}

void SettingsDialog::okButtonClicked()
{
    settings.setValue(settings.CPrivateKeyPath,ui->lePrivateKey->text());
    settings.setValue(settings.CPublicKeyPath,ui->lePublicKey->text());
    close();
}

void SettingsDialog::onChoosePrivateKeyClicked()
{
    QString temp=QFileDialog::getOpenFileName(this, "Select private key...",ui->lePrivateKey->text());
    if(!temp.isNull())
        ui->lePrivateKey->setText(temp);
    settings.setValue(settings.CPrivateKeyPath,ui->lePrivateKey->text());
}

void SettingsDialog::onChoosePublicKeyClicked()
{
    QString temp=QFileDialog::getOpenFileName(this, "Select public key...",ui->lePublicKey->text());
    if(!temp.isNull())
        ui->lePublicKey->setText(temp);
    settings.setValue(settings.CPublicKeyPath,ui->lePublicKey->text());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

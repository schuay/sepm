#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>

using namespace sdcc;

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

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
    ui->lePrivateKey->setText(QFileDialog::getOpenFileName(this, "Select private key..."));

}

void SettingsDialog::onChoosePublicKeyClicked()
{
    ui->lePublicKey->setText(QFileDialog::getOpenFileName(this, "Select public key..."));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

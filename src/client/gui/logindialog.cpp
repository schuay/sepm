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

Q_DECLARE_METATYPE(QSharedPointer<Session>);

LoginDialog::LoginDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginDialog), busyWorkers(0)
{
    ui->setupUi(this);
    qRegisterMetaType<QSharedPointer<Session> >("QSharedPointer<Session>");

    connect(ui->pbTestConnection, SIGNAL(clicked()), this, SLOT(onTestConnectionClicked()));
    connect(sdcc::SessionManager::getInstance(), SIGNAL(testConnectionCompleted(bool,QString)),
            this, SLOT(onTestConnectionCompleted(bool,QString)));
    connect(ui->pbRegister, SIGNAL(clicked()), this, SLOT(onRegisterUserClicked()));
    connect(sdcc::SessionManager::getInstance(), SIGNAL(registerCompleted(bool,QString)),
            this, SLOT(onRegisterUserCompleted(bool,QString)));
    connect(ui->pbServerCert, SIGNAL(clicked()), this, SLOT(onChooseServerCertClicked()));
    connect(ui->pbPublicKey, SIGNAL(clicked()), this, SLOT(onChoosePublicKeyClicked()));
    connect(ui->pbPrivateKey, SIGNAL(clicked()), this, SLOT(onChoosePrivateKeyClicked()));
    connect(ui->pbConnect, SIGNAL(clicked()), this, SLOT(onConnectClicked()));
    connect(SessionManager::getInstance(),
            SIGNAL(loginCompleted(QSharedPointer<Session>,bool,QString)),
            this,
            SLOT(onConnectFinished(QSharedPointer<Session>,bool,QString)));

    progressBar = new QProgressBar(this);
    progressBar->setTextVisible(false);
    statusBar()->addPermanentWidget(progressBar, 0);

    ui->leServerCert->setText(this->settings.getValue(settings.CCertPath).toString());
    ui->lePublicKey->setText(this->settings.getValue(settings.CPublicKeyPath).toString());
    ui->lePrivateKey->setText(this->settings.getValue(settings.CPrivateKeyPath).toString());
    ui->leUsername->setText(this->settings.getValue(settings.CUsername).toString());

    QRect rect = QApplication::desktop()->availableGeometry();
    this->move(rect.center() - this->rect().center());
}

void LoginDialog::onChooseServerCertClicked()
{
    QString temp=QFileDialog::getOpenFileName(this, "Selectd_session->logout(); server certificate...",ui->leServerCert->text());
    if(!temp.isNull())
        ui->leServerCert->setText(temp);
    settings.setValue(settings.CCertPath,ui->leServerCert->text());
}

void LoginDialog::onChoosePrivateKeyClicked()
{
    QString temp=QFileDialog::getOpenFileName(this, "Select private key...",ui->lePrivateKey->text());
    if(!temp.isNull())
        ui->lePrivateKey->setText(temp);
    settings.setValue(settings.CPrivateKeyPath,ui->lePrivateKey->text());
}

void LoginDialog::onChoosePublicKeyClicked()
{
    QString temp=QFileDialog::getOpenFileName(this, "Select public key...",ui->lePublicKey->text());
    if(!temp.isNull())
        ui->lePublicKey->setText(temp);
    settings.setValue(settings.CPublicKeyPath,ui->lePublicKey->text());
}

void LoginDialog::onTestConnectionClicked()
{
    if (ui->leServer->text().isEmpty()) {
        QMessageBox::warning(this, "Missing arguments",
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
            || ui->leServerCert->text().isEmpty()
            || ui->lePassword->text().isEmpty()) {
        QMessageBox::warning(this, "Missing arguments",
                             QString("Please make sure the username, password, public key, "
                                     "server and server certificate fields are filled in."));
        return;
    }

    try {
        User u(ui->leUsername->text() + QString("@") + ui->leServer->text(),
               ui->lePublicKey->text());

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
            || ui->leServerCert->text().isEmpty()
            || ui->lePassword->text().isEmpty()) {
        QMessageBox::warning(this, "Missing arguments",
                             QString("Please make sure the username, password, public key, "
                                     "server and server certificate fields are filled in."));
        return;
    }

    try {
        LoginUser u(ui->leUsername->text() + QString("@") + ui->leServer->text(),
                    ui->lePublicKey->text(), ui->lePrivateKey->text());

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
    if(success) {
        AppWindow * win = new AppWindow(0, session);
        win->show();
        hide();
        connect(win, SIGNAL(destroyed()), this, SLOT(show()));
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
    settings.setValue(settings.CUsername,ui->leUsername->text());
    delete ui;
}

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <session.h>
#include "settings.h"
#include <QMainWindow>

using namespace sdcc;

namespace Ui
{
class LoginDialog;
}

class QProgressBar;

class LoginDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void onTestConnectionClicked();
    void onTestConnectionCompleted(bool success, const QString &msg);
    void onRegisterUserClicked();
    void onRegisterUserCompleted(bool success, const QString &msg);
    void onConnectClicked();
    void onConnectFinished(QSharedPointer<Session> session, bool success, const QString &msg);

    void onChooseServerCertClicked();
    void onChoosePrivateKeyClicked();
    void onChoosePublicKeyClicked();

private:
    void addWorker();
    void removeWorker();

    Ui::LoginDialog *ui;
    QProgressBar *progressBar;
    sdc::Settings settings;
    unsigned int busyWorkers;
};

#endif // LOGINDIALOG_H

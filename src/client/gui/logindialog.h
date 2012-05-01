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



#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QMainWindow>

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

    void onChooseServerCertClicked();
    void onChoosePrivateKeyClicked();
    void onChoosePublicKeyClicked();

private:
    void addWorker();
    void removeWorker();

    Ui::LoginDialog *ui;
    QProgressBar *progressBar;

    unsigned int busyWorkers;
};

#endif // LOGINDIALOG_H

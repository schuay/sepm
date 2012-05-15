#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "session.h"
#include "settings.h"
#include <QDialog>

using namespace sdcc;

namespace Ui
{
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
private slots:
    void onChoosePrivateKeyClicked();
    void onChoosePublicKeyClicked();
    void okButtonClicked();
private:
    Ui::SettingsDialog *ui;
    sdc::Settings settings;
};

#endif // SETTINGS_H

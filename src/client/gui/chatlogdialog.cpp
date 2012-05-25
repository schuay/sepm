#include "chatlogdialog.h"
#include "ui_chatlogdialog.h"

ChatLogDialog::ChatLogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatLogDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("View Chatlogs");
}

ChatLogDialog::~ChatLogDialog()
{
    delete ui;
}

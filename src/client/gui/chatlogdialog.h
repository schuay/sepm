#ifndef CHATLOG_H
#define CHATLOG_H

#include <QDialog>

namespace Ui
{
class ChatLogDialog;
}

class ChatLogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatLogDialog(QWidget *parent = 0);
    ~ChatLogDialog();

private:
    Ui::ChatLogDialog *ui;
};

#endif // CHATLOG_H

#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDeclarativeView>
namespace sdcc
{
class LoginWindow : public QDeclarativeView
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = 0);

signals:

public slots:
    Q_INVOKABLE QString fileDialog(const QString &name = QString(),
                                   const QString &dir = QString());

};
}

#endif // LOGINWINDOW_H

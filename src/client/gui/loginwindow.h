#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDeclarativeView>

#define CRT_FILTER tr("Certificate Files (*.pem *.crt);;Key Files(*.key);;All Files (*)")
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
                                   const QString &dir = QString(),
                                   const QString &types = QString(CRT_FILTER));
    void testServer(const QString &serverUri);
};
}

#endif // LOGINWINDOW_H

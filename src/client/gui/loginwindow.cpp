#include "loginwindow.h"
#include <QFileDialog>
#include <QDeclarativeContext>

namespace sdcc
{
LoginWindow::LoginWindow(QWidget *parent) :
    QDeclarativeView(QUrl("qrc:/qml/Login.qml"), parent)
{
    setWindowTitle("SDC");
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
    rootContext()->setContextProperty("context", this);
}

QString LoginWindow::fileDialog(const QString &name, const QString &dir)
{
    return QFileDialog::getOpenFileName(this, name, dir);
}
}

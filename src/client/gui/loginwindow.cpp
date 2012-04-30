#include "loginwindow.h"
#include "sessionmanager.h"
#include <QFileDialog>
#include <QDeclarativeContext>
#include <QGraphicsItem>
#include <QMetaObject>

namespace sdcc
{
LoginWindow::LoginWindow(QWidget *parent) :
    QDeclarativeView(QUrl("qrc:/qml/Login.qml"), parent)
{
    setWindowTitle("SDC");
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
    rootContext()->setContextProperty("context", this);
    QObject * root = dynamic_cast<QObject*>(rootObject());
    connect(root, SIGNAL(testServer(const QString&, const QString&)), SLOT(testServer(const QString&, const QString&)));
    root->connect(SessionManager::getInstance(), SIGNAL(testConnectionCompleted(bool, const QString&)), SLOT(serverTestCallback(bool, QString&)));
}

QString LoginWindow::fileDialog(const QString &name, const QString &dir, const QString &types)
{
    return QFileDialog::getOpenFileName(this, name, dir, types);
}

void LoginWindow::testServer(const QString &serverUri)
{
    SessionManager::testConnection(serverUri);
}
}

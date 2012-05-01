#include "loginwindow.h"

#include <QtDeclarative>

#include "sessionmanager.h"
#include "QsLog.h"

namespace sdcc
{

LoginWindow::LoginWindow(QWidget *parent) :
    QDeclarativeView(QUrl("qrc:/qml/Login.qml"), parent)
{
    QLOG_TRACE() << "SDC client starting up...";

    setWindowTitle("SDC");
    setResizeMode(QDeclarativeView::SizeRootObjectToView);

    rootContext()->setContextProperty("context", this);
    QObject * root = dynamic_cast<QObject*>(rootObject());

    connect(root, SIGNAL(serverTest(QString)), SLOT(serverTest(QString)));
    connect(SessionManager::getInstance(),
            SIGNAL(testConnectionCompleted(bool,QString)),
            SLOT(serverTestCallback(bool,QString)));
}

QString LoginWindow::fileDialog(const QString &name, const QString &dir, const QString &types)
{
    return QFileDialog::getOpenFileName(this, name, dir, types);
}

void LoginWindow::serverTest(const QString & uri)
{
    QLOG_TRACE() << "Testing connection to " << uri;
    SessionManager::getInstance()->testConnection(uri);
}

void LoginWindow::serverTestCallback(bool success, const QString &message)
{
    QLOG_TRACE() << "Received reply to connection test: success " << success
                 << ", message " << message;
    QMetaObject::invokeMethod(rootObject(),
                              "serverTestCallback",
                              Q_ARG(QVariant, QVariant(success)),
                              Q_ARG(QVariant, QVariant(message)));
}

}

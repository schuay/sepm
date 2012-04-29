#include "loginwindow.h"
#include <QApplication>

using namespace sdcc;

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(client_gui_qml);
    LoginWindow win;
    win.show();
    return a.exec();
}

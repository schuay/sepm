#include <QtGui>

/**
 * Just a little tester for anything in the backend. Feel free to modify and
 * destroy it however you like to...
 **/

class test : public QObject
{
    Q_OBJECT

public slots:

    void report(bool success, const QString &msg) {
        printf("got %d %s\n", success, msg.toStdString().c_str());
        exit(0);
    }
};

#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(argc,argv);
    if(w.isInitialized()) {
        w.show();
        return a.exec();
    } else {
        return 1;
    }
}

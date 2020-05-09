#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow;
    MainWindow copy(*w);
    delete w;
    copy.show();

    return a.exec();
}

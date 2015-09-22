#include <iostream>
#include <QApplication>

#include "mainwindow.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
//    cout << "Test" << endl;
//    return 0;
}

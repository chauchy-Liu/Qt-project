#include "mainwindow.h"
#include <QtCore5Compat/QTextCodec> 
#include <QApplication>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    MainWindow w2;
    QObject::connect(&w2, &MainWindow::mySignal, &w2, &MainWindow::mySlot);
    //手动通过槽函数触发信号, 
    w2.mySlot(200);
    //手动通过槽函数触发信号, 
    emit w2.mySignal(100);
    return a.exec();
}

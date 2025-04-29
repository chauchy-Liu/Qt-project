#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnCloseIcon_clicked();
    void btnOldConnectFunc();
    void btnNewConnectFunc();
    //lambda匿名槽函数方式不需要声明

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

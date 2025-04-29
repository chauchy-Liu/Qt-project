#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QFile file;
    QString preEncodeName;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void zoomIn();
    void zoomOut();

private slots:
    void on_btnCloseIcon_clicked();
    void btnOldConnectFunc();
    void btnNewConnectFunc();
    void on_btnOpenIcon_clicked();
    void on_btnSaveIcon_clicked();
    void oncurrentIndexChanged(int index);
    void onCursorPositionChanged();
    //lambda匿名槽函数方式不需要声明

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

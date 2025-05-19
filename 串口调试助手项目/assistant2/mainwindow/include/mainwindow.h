#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include "comboxEvent.h"
#include <QPushButton>
#include "customThread.h"

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
    void on_closePushButton_clicked();
    void on_sendPushButton_clicked();
    void onSerialDataReadyToRead();
    void on_timeSendCheckBox_clicked(bool checked);
    void on_clearPushButton_clicked();
    void on_saveRevPushButton_clicked();
    void refreshStatusTime();
    void on_revTimeCheckBox_clicked(bool checked);
    void on_hexShowCheckBox_clicked(bool checked);
    void on_hideBoardPushButton_clicked(bool checked);
    void on_hideHisPushButton_clicked(bool checked);
    void on_chuanComboBox_refresh();
    void on_sendSerialTextCommand();
    // void on_pushButton1_clicked();
    // void on_pushButton2_clicked();
    // void on_pushButton3_clicked();
    // void on_pushButton4_clicked();
    // void on_pushButton5_clicked();
    // void on_pushButton6_clicked();
    // void on_pushButton7_clicked();
    // void on_pushButton8_clicked();
    // void on_pushButton9_clicked();
    // void on_pushButton10_clicked();
    // void on_pushButton11_clicked();
    // void on_pushButton12_clicked();
    // void on_pushButton13_clicked();
    void on_xunhuanCheckBox_clicked(bool checked);
    void xuhuanSend();
    void on_resetPushButton_clicked();
    void on_loadPushButton_clicked();
    void on_savePushButton_clicked();

private:
    Ui::MainWindow *ui;
    //定义串口对象
    QSerialPort* serialPort;
    //状态栏写入的字符数
    int writeCount;
    //状态栏读取的字符数
    int readCount;
    //历史记录
    QString sendHis;
    //定义串口是否打开
    bool isSerialPortOpen;
    //定时发送计时器
    QTimer* timer;
    //显示时间格式
    QString myTime;
    //定义是否勾选定时发送
    bool isTimeBoxChecked;
    //定义pushbutton数组
    QList<QPushButton*> pushButtonList;
    //定义循环发送定时器
    QTimer* xunhuanTimer;
    //循环发送计数
    int xunhuanCount;
    //线程对象
    CustomThread* myThread;


    void getSystemTime();

};
#endif // MAINWINDOW_H

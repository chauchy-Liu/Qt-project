#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

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
    // void on_chuanComboBox_activated(int index);

private:
    Ui::MainWindow *ui;
    QSerialPort* serialPort;
    int writeCount;
    int readCount;
    QString sendHis;
    bool isSerialPortOpen;
    QTimer* timer;
    QString myTime;
    bool isTimeBoxChecked;


    void getSystemTime();

};
#endif // MAINWINDOW_H

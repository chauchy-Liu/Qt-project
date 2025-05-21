#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include "socketComboBox.h"

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

    QTcpServer* server;

private:
    Ui::MainWindow *ui;
    int socketIndex;

private slots:
    void on_newClient_connect();
    void on_startPushButton_clicked();
    void on_readyRead_handler();
    void on_disconnected_handler();
    void on_socketStateChanged_handler(QAbstractSocket::SocketState socketState);
    void on_sendPushButton_clicked();
    void on_socketComboBox_handler();
    void on_socketComboBox_activated(int index);
    void on_stopPushButton_clicked();
    void on_interruptPushButton_clicked();
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTextEdit>

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

private:
    Ui::MainWindow *ui;
    QTcpSocket* client;

    void setInsertChartFormat(QTextEdit* textEdit, Qt::GlobalColor color, Qt::AlignmentFlag align);

private slots:
    void on_connectPushButton_clicked();
    void on_receiveData_handler();
    void on_sendPushButton_clicked();
    void on_interruptPushButton_clicked();
    void on_stateChange_handler();
};
#endif // MAINWINDOW_H

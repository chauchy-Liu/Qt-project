#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
    QTimer* timer;
    float currentAngleGlobal=0;
    //刻度范围
    int maxAngle = 210;
    int minAngle = -30;//330;
    //顺时针和逆时针切换
    int mark = 0;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <tuple>

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
    //高速刻度范围
    int maxHighSpeed = -30;
    int minHighSpeed = 50;//330;
    //顺时针和逆时针切换
    int mark = 0;

    QRect initCanvas(QPainter &painter);

    int drawCircle(QPainter &painter, QRect &rect, int radius, QPen &pen, float ratio);

    std::tuple<int, int> drawTick(QPainter &painter, QRect &rect, QPen &pen, int radius);

     std::tuple<float, float> drawArrow(QPainter &painter, QRect &rect, int &radius, int &smallRadius);

    void drawPie(QPainter &painter, QRect &rect, int &radius, float &arrowRadius, float &currentAngle);

    void drawSpeed(QPainter &painter, QRect &rect, int &textRatio, int &tickInterval, float &currentAngle,
                   int &smallRadius);

    void drawShinePie(QPainter &painter, QRect &rect, float radius, float arrowRadius, float currentAngle);

    void drawLogol(QPainter &painter, QRect &rect, float radius);
};
#endif // MAINWINDOW_H

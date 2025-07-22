#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>

using namespace QtCharts;//或者用宏 Qt_CHARTS_USE_NAMESPACE
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
    QChartView *chartView; //视图组件架构，类似于QGraphicsView
    QChart *chart;
    void createChart();// 创建图表
    QLineSeries* series1;//空曲线对象
    QLineSeries* series2;
    QValueAxis *axisX;//坐标轴
    QValueAxis *axisY;//坐标轴
};
#endif // MAINWINDOW_H

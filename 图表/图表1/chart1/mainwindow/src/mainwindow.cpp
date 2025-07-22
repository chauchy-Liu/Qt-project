#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineSeries>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("图表窗口");
    createChart();//调用图表
}

MainWindow::~MainWindow()
{
    delete ui;
    delete chart;
    delete chartView;
    delete series1;
    delete series2;
    delete axisX;
    delete axisY;
}

void MainWindow::createChart() {
    //创建图表
    chartView = new QChartView(this);
    chart = new QChart();
    chartView->setChart(chart);
    chart->setTitle("sin、cos函数图表测试");
    this->setCentralWidget(chartView);
    //创建曲线序列
    series1 = new QLineSeries();
    series2 = new QLineSeries();
    series1->setName("sin曲线");
    series2->setName("cos曲线");
    chart->addSeries(series1);
    chart->addSeries(series2);
    //曲线序列添值
    qreal t=0, y1, y2, intV=0.1;
    int count = 100;//3600;
    for (int i=0; i < count; i++) {
        // qreal rad = t*M_PI/180.;
        // y1 = qSin(rad);
        // series1->append(rad, y1);
        // y2 = qCos(rad);
        // series2->append(rad, y2);
        y1 = qSin(t);
        series1->append(t, y1);
        y2 = qCos(t);
        series2->append(t, y2);
        t = t+intV;
    }
    //创建坐标轴
    axisX = new QValueAxis();
    axisY = new QValueAxis();
    // axisX->setRange(0, t*M_PI/180.);
    axisX->setRange(0, 10);
    axisX->setTitleText("time (secs)");
    axisY->setRange(-1.5, 1.5);
    axisY->setTitleText("values");
    chart->setAxisX(axisX, series1);
    chart->setAxisY(axisY, series1);
    chart->setAxisX(axisX, series2);
    chart->setAxisY(axisY, series2);

}

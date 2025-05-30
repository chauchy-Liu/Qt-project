#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setLayout(ui->regionGridLayout);
    startAngleGlobal = 0;
    timer = new QTimer(this);
    timer->setInterval(500);
    timer->start();
    connect(timer, &QTimer::timeout, [=](){update();});
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    QPen pen(QColor(0, 255, 0), 3);
    // painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(pen);
    painter.save();

    QBrush brush(QColor(0, 0, 0, 220));
    painter.setBrush(brush);

    QRect rect =  ui->centralwidget->rect(); //(0,0, 60, 60);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect);
    painter.restore();

    int circleNum = 5;
    int side = (rect.width()/2<rect.height()/2)?(rect.width()/2):(rect.height()/2);
    int interval = side/circleNum;
    for (int i=1; i<=circleNum; i++) {
        if (side-i*interval>=0) {
            painter.drawEllipse(rect.center(), i*interval, i*interval);
        }
    }
    //除不尽所以有露头
    // painter.drawLine(rect.center().x()-side, rect.center().y(), rect.center().x()+side, rect.center().y());
    // painter.drawLine(rect.center().x(), rect.center().y()+side, rect.center().x(), rect.center().y()-side);
    painter.drawLine(rect.center().x()-interval*circleNum, rect.center().y(), rect.center().x()+interval*circleNum, rect.center().y());
    painter.drawLine(rect.center().x(), rect.center().y()+interval*circleNum, rect.center().x(), rect.center().y()-interval*circleNum);
    //画扇形
    if (startAngleGlobal > 360) startAngleGlobal -= 360;
    int startAngle = startAngleGlobal;
    int spanAngle = -90;
    QRect rectPie(rect.center().x()-interval*circleNum, rect.center().y()-interval*circleNum, interval*circleNum*2, interval*circleNum*2);
    QConicalGradient conicalGradient(rect.center(), startAngle);
    conicalGradient.setColorAt(0.0, QColor(0, 255, 0, 0));
    conicalGradient.setColorAt(0.8, QColor(0, 255, 0, 0));
    conicalGradient.setColorAt(0.9, QColor(0, 255, 0, 50));
    conicalGradient.setColorAt(1.0, QColor(0, 255, 0, 100));
    QBrush brushPie(conicalGradient);
    painter.setBrush(brushPie);
    painter.setPen(Qt::NoPen);
    painter.drawPie(rectPie, startAngle*16, spanAngle*16);
    startAngleGlobal += abs(30);

}

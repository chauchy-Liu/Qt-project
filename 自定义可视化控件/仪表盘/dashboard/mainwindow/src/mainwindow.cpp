#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setLayout(ui->centralGL);
    timer = new QTimer(this);
    timer->setInterval(500);
    timer->start();
    connect(timer, &QTimer::timeout, [=]() {
        if (mark == 0) { //顺时针
            currentAngleGlobal -= 30;
            // if ()
            if (currentAngleGlobal < minAngle) {
                currentAngleGlobal = minAngle;
                mark = 1;
            }
        }
        if (mark==1) { //逆时针
            currentAngleGlobal += 40;
            if (currentAngleGlobal > maxAngle) {
                currentAngleGlobal = maxAngle;
                mark = 0;
            }
        }
        update();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QMainWindow::paintEvent(event);
    QPainter painter(this);
    //绘图区域
    //不要用mainwindow的rect(),而是mainwindow里的centralwidget的rect()
    QRect rect = ui->centralwidget->rect();
    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);
    //底色黑色
    painter.setBrush(Qt::black);
    painter.drawRect(rect);
    painter.setBrush(Qt::BrushStyle::NoBrush);
    //坐标系平移到中心, 矩形内切圆
    // painter.translate(rect().center());
    //1.画大圆
    int radius = (rect.center().x()>rect.center().y())?rect.center().y():rect.center().x();
    // painter.setPen(Qt::red);
    //径向渐变色
    QRadialGradient radGradient(rect.center(), radius);
    //颜色停靠点
    radGradient.setColorAt(0.0, QColor(255,0,0, 50));
    radGradient.setColorAt(1.0, QColor(255,0,0, 250));
    painter.drawEllipse(rect.center(), radius, radius);
    //新Brush
    QBrush brush(radGradient);
    painter.setBrush(brush);
    painter.drawEllipse(rect.center(), radius, radius);
    painter.setBrush(Qt::BrushStyle::NoBrush);
    //2.画小圆
    float ratio = 1./5;
    int smallRadius = (int)(radius*ratio);
    qDebug() << "smallRadius:" << smallRadius << ", radio: " << ratio << ", Radius: " << radius;
    QPen pen(Qt::white);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawEllipse(rect.center(), smallRadius, smallRadius);
    painter.setPen(Qt::NoPen);
    //3.画刻度
    int textNum = 8;//必须被360整除
    // qDebug() << "sin30: " << sin(30*M_PI/180);
    pen.setWidth(2);
    painter.setPen(pen);
    float bigTick = (1-0.9)*radius;
    float midTick = (1-0.93)*radius;
    float smallTick = (1-0.96)*radius;
    float textRadius = 0.8*radius;
    //刻度范围
    // int maxAngle = 210;
    // int minAngle = -30;//330;
    int tickInterval = (int)(360./textNum/10);
    int tickCount = 0;
    for (int i = maxAngle; i >= minAngle; i-=tickInterval) {
        float xOut = cos(i*M_PI/180)*radius+rect.center().x();
        float yOut = -sin(i*M_PI/180)*radius+rect.center().y();
        float xIn;
        float yIn;
        float xText;
        float yText;
        if (tickCount%5 == 0 && tickCount%10 == 0) {
            xIn = cos(i*M_PI/180)*(radius-bigTick)+rect.center().x();
            yIn = -sin(i*M_PI/180)*(radius-bigTick)+rect.center().y();
            xText = cos(i*M_PI/180)*textRadius+rect.center().x();
            yText = -sin(i*M_PI/180)*textRadius+rect.center().y();
            //作为文字矩形框的中心点
            QPointF pointText(xText, yText);
            //半长，半宽
            float halfWidth = 1.5*bigTick;
            float halfHeight = 1.5*bigTick;
            QFont font;
            font.setPointSize(bigTick);
            font.setWeight(QFont::Thin);
            painter.setFont(font);
            QRectF rectText(pointText.x()-halfWidth, pointText.y()-halfHeight, 2*halfWidth, 2*halfHeight);
            painter.drawText(rectText, Qt::AlignCenter, QString::number(tickCount));
        }else if (tickCount%5 == 0 && tickCount%10 != 0) {
            xIn = cos(i*M_PI/180)*(radius-midTick)+rect.center().x();
            yIn = -sin(i*M_PI/180)*(radius-midTick)+rect.center().y();
        }else {
            xIn = cos(i*M_PI/180)*(radius-smallTick)+rect.center().x();
            yIn = -sin(i*M_PI/180)*(radius-smallTick)+rect.center().y();
        }
        tickCount++;
        QPointF pointOut(xOut, yOut);
        QPointF pointIn(xIn, yIn);
        painter.drawLine(pointIn, pointOut);
    }
    //4.画指针
    // if (currentAngleGlobal > maxAngle || currentAngleGlobal < minAngle) {
    //     currentAngleGlobal = maxAngle;
    // }
    float currentAngle = currentAngleGlobal;//190;
    float arrowRadius = 0.68*radius;
    float xArrowOut = cos(currentAngle*M_PI/180)*arrowRadius+rect.center().x();
    float yArrowOut = -sin(currentAngle*M_PI/180)*arrowRadius+rect.center().y();
    float xArrowIn = cos(currentAngle*M_PI/180)*smallRadius+rect.center().x();
    float yArrowIn = -sin(currentAngle*M_PI/180)*smallRadius+rect.center().y();
    QPointF pointArrowOut(xArrowOut, yArrowOut);
    QPointF pointArrowIn(xArrowIn, yArrowIn);
    painter.drawLine(pointArrowIn, pointArrowOut);
    //5.画扇形
    QRectF rectPie(rect.center().x()-arrowRadius, rect.center().y()-arrowRadius, arrowRadius*2, arrowRadius*2);
    painter.setPen(Qt::NoPen);
    QRadialGradient arrowRG(rectPie.center(), arrowRadius);
    arrowRG.setColorAt(0.0, QColor(255,153,18, 0));
    arrowRG.setColorAt(0.3, QColor(255,153,18, 0));
    arrowRG.setColorAt(1.0, QColor(255,153,18, 200));
    QBrush arrowBrush(arrowRG);
    painter.setBrush(arrowBrush);
    painter.drawPie(rectPie, currentAngle*16, (maxAngle-currentAngle)*16);
    //6.显示刻度值
    //作为文字矩形框的中心点
    QPointF pointText(rect.center().x(), rect.center().y());
    //半长，半宽
    float halfWidth = smallRadius;
    float halfHeight = smallRadius;
    QFont font;
    font.setPointSize(smallRadius*0.8);
    font.setWeight(QFont::Bold);
    painter.setFont(font);
    QRectF rectText(pointText.x()-halfWidth, pointText.y()-halfHeight, 2*halfWidth, 2*halfHeight);
    painter.setPen(Qt::white);
    painter.drawText(rectText, Qt::AlignCenter, QString::number((maxAngle-currentAngle)/tickInterval));
}

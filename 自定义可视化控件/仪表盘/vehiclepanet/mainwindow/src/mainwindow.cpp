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
    setFixedSize(800, 600);
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

    //设置画布
    QRect rect = initCanvas(painter);
    //1.画大圆
    int radius = (rect.center().x()>rect.center().y())?rect.center().y():rect.center().x();
    radius =(int)(radius*0.7);
    // painter.setPen(Qt::red);
    // //径向渐变色
    // QRadialGradient radGradient(rect.center(), radius);
    // //颜色停靠点
    // radGradient.setColorAt(0.0, QColor(255,0,0, 50));
    // radGradient.setColorAt(1.0, QColor(255,0,0, 250));
    // painter.drawEllipse(rect.center(), radius, radius);
    // //新Brush
    // QBrush brush(radGradient);
    // painter.setBrush(brush);
    // painter.drawEllipse(rect.center(), radius, radius);
    painter.setBrush(Qt::BrushStyle::NoBrush);
    //2.画小圆
    QPen pen(QColor(255,255,255,0));

    float ratio = 1./5;
    int smallRadius = drawCircle(painter, rect, radius, pen, ratio);
    //3.画刻度
    pen.setColor(Qt::white);
    std::tuple<int, int > paramTuple = drawTick(painter, rect, pen, radius);
    int textRatio = std::get<0>(paramTuple);
    int tickInterval = std::get<1>(paramTuple);
    //4.画指针
    std::tuple<float, float> arrowTuple = drawArrow(painter, rect, radius, smallRadius);
    float currentAngle = std::get<0>(arrowTuple);
    float arrowRadius = std::get<1>(arrowTuple);
    //5.画扇形
    drawPie(painter, rect, radius, arrowRadius, currentAngle);
    //6.中圆渐变
    // painter.setPen(Qt::NoPen);
    pen.setColor(QColor(255, 255, 255, 0));
    ratio = 1./3;
    //径向渐变色
    QRadialGradient radGradient(rect.center(), ratio*radius);
    //颜色停靠点
    radGradient.setColorAt(0.0, QColor(255,0,0, 200));
    radGradient.setColorAt(1.0, QColor(0,0, 0, 100));
    //新Brush
    QBrush brush(radGradient);
    painter.setBrush(brush);
    int midRadius = drawCircle(painter, rect, radius, pen, ratio);
    //7.内圆黑
    painter.setBrush(Qt::black);
    ratio = 1./5;
    smallRadius = drawCircle(painter, rect, radius, pen, ratio);
    //8.显示刻度值
    painter.setPen(Qt::white);
    drawSpeed(painter, rect, textRatio, tickInterval, currentAngle, smallRadius);
    //9.发光外环
    drawShinePie(painter, rect, radius*1.1, radius*1.1, minAngle);
    //10.汽车logol
    drawLogol(painter, rect, radius);
}

//
QRect MainWindow::initCanvas(QPainter &painter) {
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
    return rect;
}
int MainWindow::drawCircle(QPainter& painter, QRect& rect, int radius, QPen& pen, float ratio) {
    int smallRadius = (int)(radius*ratio);
    // qDebug() << "smallRadius:" << smallRadius << ", radio: " << ratio << ", Radius: " << radius;
    // QPen pen(Qt::white);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawEllipse(rect.center(), smallRadius, smallRadius);
    painter.setPen(Qt::NoPen);
    return smallRadius;
}

std::tuple<int, int> MainWindow::drawTick(QPainter& painter,QRect& rect, QPen& pen, int radius) {
    int textNum = 13;//必须被360整除
    int smallTickNum = 5;
    int textRatio = 4;
    // qDebug() << "sin30: " << sin(30*M_PI/180);
    pen.setWidth(2);
    painter.setPen(pen);
    float bigTick = (1-0.9)*radius;
    // float midTick = (1-0.93)*radius;
    float smallTick = (1-0.96)*radius;
    float textRadius = 0.8*radius;
    //刻度范围
    // int maxAngle = 210;
    // int minAngle = -30;//330;
    int tickInterval = (int)((maxAngle-minAngle)/(textNum-1.)/smallTickNum);
    int tickCount = 0;
    for (int i = maxAngle; i >= minAngle; i-=tickInterval) {
        float xOut = cos(i*M_PI/180)*radius+rect.center().x();
        float yOut = -sin(i*M_PI/180)*radius+rect.center().y();
        float xIn;
        float yIn;
        float xText;
        float yText;
        if (tickCount%5 == 0) {
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
            font.setWeight(QFont::Bold);
            painter.setFont(font);
            painter.save();
            painter.translate(pointText);
            painter.rotate(-(i-90));
            QRectF rectText(-halfWidth, -halfHeight, 2*halfWidth, 2*halfHeight);
            painter.drawText(rectText, Qt::AlignCenter, QString::number(tickCount*textRatio));
            // qDebug()<<QString::number(tickCount*textRatio);
            painter.restore();
        }else {
            xIn = cos(i*M_PI/180)*(radius-smallTick)+rect.center().x();
            yIn = -sin(i*M_PI/180)*(radius-smallTick)+rect.center().y();
        }
        if (i >= maxHighSpeed && i <= minHighSpeed) {
            pen.setColor(Qt::red);
        }
        tickCount++;
        QPointF pointOut(xOut, yOut);
        QPointF pointIn(xIn, yIn);

        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawLine(pointIn, pointOut);
        pen.setColor(Qt::white);
        painter.setPen(pen);
    }
    return std::make_tuple(textRatio, tickInterval);
}
std::tuple<float, float> MainWindow::drawArrow(QPainter& painter, QRect& rect, int& radius, int& smallRadius) {
    // if (currentAngleGlobal > maxAngle || currentAngleGlobal < minAngle) {
    //     currentAngleGlobal = maxAngle;
    // }
    float currentAngle = currentAngleGlobal;//(maxAngle-currentAngleGlobal)*4;//190;
    float arrowRadius = 0.68*radius;
    float epsilon = 5.1;
    float xArrowOut = cos(currentAngle*M_PI/180)*arrowRadius+rect.center().x();
    float yArrowOut = -sin(currentAngle*M_PI/180)*arrowRadius+rect.center().y();
    float xArrowIn1 = cos((currentAngle-epsilon)*M_PI/180)*smallRadius+rect.center().x();
    float yArrowIn1 = -sin((currentAngle-epsilon)*M_PI/180)*smallRadius+rect.center().y();
    float xArrowIn2 = cos((currentAngle+epsilon)*M_PI/180)*smallRadius+rect.center().x();
    float yArrowIn2 = -sin((currentAngle+epsilon)*M_PI/180)*smallRadius+rect.center().y();
    QPointF pointArrowOut(xArrowOut, yArrowOut);
    QPointF pointArrowIn1(xArrowIn1, yArrowIn1);
    QPointF pointArrowIn2(xArrowIn2, yArrowIn2);
    QPointF pointArrows[3] = {pointArrowOut, pointArrowIn1, pointArrowIn2};
    // painter.drawLine(pointArrowIn, pointArrowOut);
    painter.setBrush(Qt::white);
    painter.drawPolygon(pointArrows, 3);
    return std::make_tuple(currentAngle, arrowRadius);
}

void MainWindow::drawPie(QPainter& painter, QRect& rect, int& radius, float& arrowRadius, float& currentAngle) {
    float ratio = 1.08;
    QRectF rectPie(rect.center().x()-radius*ratio, rect.center().y()-radius*ratio, radius*ratio*2, radius*ratio*2);
    painter.setPen(Qt::NoPen);
    QRadialGradient arrowRG(rectPie.center(), radius*ratio);//arrowRadius
    arrowRG.setColorAt(0.0, QColor(255,153,18, 0));
    arrowRG.setColorAt(0.3, QColor(255,153,18, 0));
    arrowRG.setColorAt(1.0, QColor(255,153,18, 100));
    QBrush arrowBrush(arrowRG);
    painter.setBrush(arrowBrush);
    painter.drawPie(rectPie, currentAngle*16, (maxAngle-currentAngle)*16);
}

void MainWindow::drawSpeed(QPainter& painter, QRect& rect, int& textRatio, int& tickInterval, float& currentAngle, int& smallRadius) {
    float value = (maxAngle-currentAngle)*textRatio/tickInterval;
    //作为文字矩形框的中心点
    QPointF pointText(rect.center().x(), rect.center().y());
    //半长，半宽
    float halfWidth = smallRadius;
    float halfHeight = smallRadius;
    float offsize = 1.5; //0到2
    QFont font;
    font.setPointSize(smallRadius*0.8);
    font.setWeight(QFont::Bold);
    painter.setFont(font);
    QRectF rectText(pointText.x()-halfWidth, pointText.y()-halfHeight, 2*halfWidth, offsize*halfHeight);
    painter.setPen(Qt::white);
    QString valueString = QString::number(value);
    painter.drawText(rectText, Qt::AlignCenter, valueString);
    QRectF rectTextUnit(pointText.x()-halfWidth/2, pointText.y()+(offsize-1), halfWidth, halfHeight);
    font.setPointSize(smallRadius*0.4);
    painter.setFont(font);
    painter.drawText(rectTextUnit, Qt::AlignCenter, "Km/h");
}

void MainWindow::drawShinePie(QPainter& painter, QRect& rect, float radius, float arrowRadius, float currentAngle) {
    QRectF rectPie(rect.center().x()-radius, rect.center().y()-radius, radius*2, radius*2);
    painter.setPen(Qt::NoPen);
    QRadialGradient arrowRG(rectPie.center(), arrowRadius);
    arrowRG.setColorAt(0.0, QColor(0,0,0, 0));
    arrowRG.setColorAt(0.3, QColor(0,0,0, 0));
    arrowRG.setColorAt(0.9, QColor(0,0,0, 10));
    arrowRG.setColorAt(0.95, QColor(255,0,0, 50));
    arrowRG.setColorAt(1.0, QColor(255,0,0, 100));
    QBrush arrowBrush(arrowRG);
    painter.setBrush(arrowBrush);
    painter.drawPie(rectPie, currentAngle*16, (maxAngle-currentAngle)*16);
}
void MainWindow::drawLogol(QPainter& painter, QRect& rect, float radius) {
    float logolCenterX = rect.center().x();
    float logolCenterY = rect.center().y()+radius*0.5;
    float halfWidth = radius*0.1;
    float halfHeight = radius*0.12;
    QRect rectLogol(logolCenterX-halfWidth, logolCenterY-halfHeight, halfWidth*2, halfHeight*2);
    painter.drawPixmap(rectLogol, QPixmap(":/logol/images/porch.png"));
}
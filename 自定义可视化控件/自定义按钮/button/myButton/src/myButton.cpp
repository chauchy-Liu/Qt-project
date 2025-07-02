//
// Created by chauchy on 25-7-1.
//

#include "myButton.h"
#include <QPainter>
#include <QDebug>


MyButton::MyButton(QWidget *parent): QWidget(parent) {
    setFixedSize(60,20);
    animation = new QPropertyAnimation(this); //动画在控件内部的移动，如果加"pos"控件在父控件移动
    //作动画的对像
    animation->setTargetObject(this);
    //起始位置
    animation->setStartValue(height()/2);
    //结束位置
    animation->setEndValue(width()-height()/2);
    //持续时间
    animation->setDuration(500);
    //设置移动速度样式,回弹效果
    animation->setEasingCurve(QEasingCurve::InOutElastic);
    //信号与槽
    connect(animation, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
        // qDebug() << "信号与槽中currentX = " << value.toInt();
        currentX = value.toInt(); //相当于让QPainterEvent作多幅画，每幅画的圆圈x坐标为currentX
        update();
    });
}

MyButton::~MyButton() {
    delete animation;
}


void MyButton::paintEvent(QPaintEvent *paint_event) {

    QPainter painter(this);
    int radius = height() / 2;
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(isOff ? offBgBrush : onBgBrush);
    painter.setPen(isOff ? offRectColor : onRectColor);
    painter.drawRoundedRect(this->rect(), radius, radius);

    painter.setBrush(isOff ? offFgBrush : onFgBrush);
    int epsilon = 5;
    QPoint cirPos;
    // if (isOff) {
    //     cirPos.setX(radius);
    //     cirPos.setY(radius);
    //     painter.drawEllipse(cirPos, radius-epsilon, radius-epsilon);
    // } else {
    //     cirPos.setX(width() - radius);
    //     cirPos.setY(radius);
    //     painter.drawEllipse(cirPos, radius-epsilon, radius-epsilon);
    // }
    cirPos.setX(currentX);
    cirPos.setY(radius);
    painter.drawEllipse(cirPos, radius-epsilon, radius-epsilon);

    painter.setPen(Qt::white);
    painter.drawText(this->rect(), Qt::AlignCenter, isOff ? offText : onText);

    QWidget::paintEvent(paint_event);
}

void MyButton::mousePressEvent(QMouseEvent *mouse_event) {
    if (mouse_event->button() == Qt::LeftButton) {
        //动画方向
        isOff ? animation->setDirection(QAbstractAnimation::Forward) : animation->setDirection(QAbstractAnimation::Backward);
        //启动动画
        animation->start();
        emit isClicked();
        emit isClickedWithParam(isOff ? true : false);
        isOff = !isOff;
        // update();
    }

    QWidget::mousePressEvent(mouse_event);
}

void MyButton::resizeEvent(QResizeEvent *resize_event) {
    //起始位置
    animation->setStartValue(height()/2);
    //结束位置
    animation->setEndValue(width()-height()/2);

    QWidget::resizeEvent(resize_event);
}

#include "mybutton.h"
#include <QPainter>

MyButton::MyButton(QWidget *parent): QWidget(parent)
{
    pic.load(":/icon/res/notebook.png");
    setFixedSize(pic.size());
    update(); //触发paintEvent
}

void MyButton::enterEvent(QEnterEvent *event)
{
    pic.load(":/icon/res/open.png");
    update();
}

void MyButton::leaveEvent(QEvent *event)
{
    pic.load(":/icon/res/notebook.png");
    update();
}

void MyButton::mousePressEvent(QMouseEvent *event)
{
    pic.load(":/icon/res/open2.png");
    update();
    emit clicked(); //发射信号
}

void MyButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), pic);
}

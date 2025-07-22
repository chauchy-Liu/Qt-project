//
// Created by chauchy on 25-7-2.
//

#include "MyGraphicsView.h"
#include <QMouseEvent>
#include <QDebug>

MyGraphicsView::MyGraphicsView(QWidget *parent): QGraphicsView(parent) {

}

void MyGraphicsView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        qDebug() <<  "clicked" << ": " << event->pos();
        QPoint pos = event->pos();// view坐标
        emit mouseClicked(pos);
    }
    QGraphicsView::mousePressEvent(event);
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    // qDebug() << "move" << ": " << event->pos();
    QPoint pos = event->pos();// view坐标
    emit mouseMovePoint(pos);
    QGraphicsView::mouseMoveEvent(event);
}

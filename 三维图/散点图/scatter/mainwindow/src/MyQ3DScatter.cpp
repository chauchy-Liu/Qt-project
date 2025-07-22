//
// Created by chauchy on 25-7-21.
//

#include "MyQ3DScatter.h"


MyQ3DScatter::MyQ3DScatter(const QSurfaceFormat *format, QWindow *parent): Q3DScatter(format, parent){
}

void MyQ3DScatter::mousePressEvent(QMouseEvent *event) {
    emit mousePress(event);
    Q3DScatter::mousePressEvent(event);
}

void MyQ3DScatter::mouseMoveEvent(QMouseEvent *event) {
    emit mouseMove(event);
    Q3DScatter::mouseMoveEvent(event);
}

void MyQ3DScatter::mouseReleaseEvent(QMouseEvent *event) {
    emit mouseRelease(event);
    Q3DScatter::mouseReleaseEvent(event);
}

void MyQ3DScatter::wheelEvent(QWheelEvent *event) {
    emit wheel(event);
    Q3DScatter::wheelEvent(event);
}

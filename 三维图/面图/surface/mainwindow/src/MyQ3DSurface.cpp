//
// Created by chauchy on 25-7-21.
//

#include "MyQ3DSurface.h"


MyQ3DSurface::MyQ3DSurface(const QSurfaceFormat *format, QWindow *parent): Q3DSurface(format, parent){
}

void MyQ3DSurface::mousePressEvent(QMouseEvent *event) {
    emit mousePress(event);
    Q3DSurface::mousePressEvent(event);
}

void MyQ3DSurface::mouseMoveEvent(QMouseEvent *event) {
    emit mouseMove(event);
    Q3DSurface::mouseMoveEvent(event);
}

void MyQ3DSurface::mouseReleaseEvent(QMouseEvent *event) {
    emit mouseRelease(event);
    Q3DSurface::mouseReleaseEvent(event);
}

void MyQ3DSurface::wheelEvent(QWheelEvent *event) {
    emit wheel(event);
    Q3DSurface::wheelEvent(event);
}

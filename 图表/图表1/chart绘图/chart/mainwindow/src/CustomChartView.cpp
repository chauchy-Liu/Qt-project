//
// Created by chauchy on 25-7-9.
//

#include "../include/CustomChartView.h"
#include <QEventPoint>

CustomChartView::CustomChartView(QWidget *parent): QChartView(parent) {
    setMouseTracking(true);
    // setInteractive(true);
    //筐选区域
    setDragMode(QGraphicsView::NoDrag);
    //初始化手势
    grabGestures();
    // //控件能够接收touch
    // setAttribute(Qt::WA_AcceptTouchEvents);
    // //针对MacOS
    // qputenv("QT_MAC_WANTS_LAYER", "1");
    // qputenv("QT_ENABLE_TRACKPAD_GESTURES", "1");
    // qputenv("QT_MAC_PROBE_TRACKPAD_GESTURE_MODE", "1");
}

CustomChartView::~CustomChartView() {
}

void CustomChartView::grabGestures() {
    QList<Qt::GestureType> gesturess;
    //关注移动，缩放和扫动手势
    gesturess << Qt::PanGesture;
    gesturess << Qt::PinchGesture;
    gesturess << Qt::SwipeGesture;
    gesturess << Qt::TapAndHoldGesture;
    foreach(Qt::GestureType gesture, gesturess)
        grabGesture(gesture);
}

void CustomChartView::mousePressEvent(QMouseEvent *event) {
    // qDebug() << event->type() << "  " << event->button() << "control Pressed" << controlPressed;
    if (event->button() == Qt::LeftButton) {
        emit signalMouseEvent(0, event);
        // setCursor(Qt::ClosedHandCursor);
    } else if (event->button() == Qt::RightButton) {
        if (controlPressed) {
            beginPoint = event->pos();
        }
        // chart()->zoomOut();
    }
    //交还基类控制权让他完成本该的任务
    QChartView::mousePressEvent(event);
}

void CustomChartView::mouseMoveEvent(QMouseEvent *event) {
    // qDebug() << "eventType: " << event->type();
    emit signalMouseEvent(1, event);
    // setCursor(Qt::ClosedHandCursor);
    QChartView::mouseMoveEvent(event);
}

void CustomChartView::mouseReleaseEvent(QMouseEvent *event) {
    // qDebug() << event->type() << "  " << event->button() << "control Pressed" << controlPressed;
    if (event->button() == Qt::LeftButton) {
        emit signalMouseEvent(2, event);
        // setCursor(Qt::ArrowCursor);
    } else if (event->button() == Qt::RightButton) {
        endPoint = event->pos();
        if (controlPressed) {
            QRectF zoomRect = QRectF(beginPoint,endPoint);
            // qDebug() << "zoomRect: " << zoomRect;
            chart()->zoomIn(zoomRect);
        }
    }
    QChartView::mouseReleaseEvent(event);
}

void CustomChartView::mouseDoubleClickEvent(QMouseEvent *event) {
    emit signalMouseEvent(3, event);
    QChartView::mouseDoubleClickEvent(event);
}

void CustomChartView::wheelEvent(QWheelEvent *event) {
    emit signalWheelEvent(event);
    QChartView::wheelEvent(event);
}

void CustomChartView::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Plus:
            chart()->zoom(1.2);
            break;
        case Qt::Key_Minus:
            chart()->zoom(0.8);
            break;
        case Qt::Key_Left:
            chart()->scroll(10,0);
            break;
        case Qt::Key_Right:
            chart()->scroll(-10,0);
            break;
        case Qt::Key_Up:
            chart()->scroll(0, -10);
            break;
        case Qt::Key_Down:
            chart()->scroll(0, 10);
            break;
        case Qt::Key_Meta:
            setDragMode(QGraphicsView::RubberBandDrag);
            setCursor(Qt::CrossCursor);
            // setCursor(Qt::ClosedHandCursor);
            controlPressed = true;
            break;
    }
    QChartView::keyPressEvent(event);
}

void CustomChartView::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Meta:
            setDragMode(QGraphicsView::NoDrag);
            setCursor(Qt::ArrowCursor);
            controlPressed = false;
            break;
    }
    QChartView::keyReleaseEvent(event);
}

bool CustomChartView::event(QEvent *event) {
    qDebug() << "eventType:" << event->type();
    if (event->type() == QEvent::Gesture) {
        return gestureEvent(static_cast<QGestureEvent*>(event));
    }
    if (event->type() == QEvent::TouchBegin) {
        touchBeginEvent(static_cast<QTouchEvent*>(event));
        return true;
    }
    if (event->type() == QEvent::TouchUpdate) {
        touchUpdateEvent(static_cast<QTouchEvent*>(event));
        return true;
    }
    return QChartView::event(event);
}

bool CustomChartView::gestureEvent(QGestureEvent *event) {
    //给到对应的处理事件
    if (QGesture* swipe = event->gesture(Qt::SwipeGesture)) {
        qDebug() << "手势类型：" << "平移";//event->type();
        swipeTriggered(static_cast<QSwipeGesture*>(swipe));
    } else if (QGesture* pan = event->gesture(Qt::PanGesture)) {
        qDebug() << "手势类型：" << "滑动";//event->type();
        panTriggered(static_cast<QPanGesture*>(pan));
    } else if (QGesture* pinch = event->gesture(Qt::PinchGesture)) {
        qDebug() << "手势类型：" << "捏合";//event->type();
        pinchTriggered(static_cast<QPinchGesture*>(pinch));
    } else if (QGesture* tapHold = event->gesture(Qt::TapAndHoldGesture)) {
        qDebug() << "手势类型：" << "长按";//event->type();
        tapHoldTriggered(static_cast<QTapAndHoldGesture*>(tapHold));
    }
    return true;

    return true;
}

void CustomChartView::tapHoldTriggered(QTapAndHoldGesture *tapAndHold) {
    qDebug() << "tapHoldTriggered";
}

void CustomChartView::panTriggered(QPanGesture *gesture) {
    qDebug() << "panTriggered滑动事件";
    switch (gesture->state()) {
        case Qt::GestureStarted:
        case Qt::GestureUpdated:
            setCursor(Qt::ClosedHandCursor);
            break;
        default:
            setCursor(Qt::ArrowCursor);
    }
    // update();
}

void CustomChartView::pinchTriggered(QPinchGesture *gesture) {
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    //缩放比例的变化记录
    if (changeFlags & QPinchGesture::ScaleFactorChanged)
    {
        m_currentStepScaleFactor = gesture->totalScaleFactor();
        qDebug() << "pinchTriggered(): zoom by" <<
            gesture->scaleFactor() << "->" << m_currentStepScaleFactor << ": center point->" << gesture->centerPoint();
        // chart()->zoom(m_currentStepScaleFactor);
        QPointF curPointView = gesture->centerPoint();
        QPointF curPointChart = chart()->mapToValue(curPointView);
        if (zoomCursorView.empty()) zoomCursorView.append(curPointView);
        if (zoomCursorChart.empty()) zoomCursorChart.append(curPointChart);
        chart()->zoom(gesture->scaleFactor());
        QPointF firstPointView = chart()->mapToPosition(zoomCursorChart[0]);
        chart()->scroll(-(curPointView.x()-firstPointView.x()) / 1., //1、曲线移动方向和图表坐标系的移动方向相反，2、图表坐标系的y轴向上而chartview组件的坐标系y轴向下
                               (curPointView.y()-firstPointView.y()) / 1.);
    }
    //手势结束后将锁防值累乘
    if (gesture->state() == Qt::GestureFinished)
    {
        qDebug() << "finished::" << m_currentStepScaleFactor;
        m_scaleFactor *= m_currentStepScaleFactor;
        m_currentStepScaleFactor = 1;
        // chart()->zoom(m_scaleFactor);
        m_scaleFactor = 1;
    }
    // update();
}

void CustomChartView::swipeTriggered(QSwipeGesture *gesture) {
    qDebug() << "swipeTriggered平移事件";
}

void CustomChartView::touchBeginEvent(QTouchEvent *event) {
    qDebug() << "touchBeginEvent----" << "touch Points: " << event->pointCount() << "state: " << event->touchPointStates();
    if (event->pointCount()==3 && event->touchPointStates()==QEventPoint::Pressed) {
        setCursor(Qt::ClosedHandCursor);
    }/* else if (event->touchPointStates()==QEventPoint::Released) {
        setCursor(Qt::ArrowCursor);
    }*/
    event->accept();
}

void CustomChartView::touchUpdateEvent(QTouchEvent *event) {
    qDebug() << "touchUpdateEvent----" << "touch Points: " << event->pointCount() << "state: " << event->touchPointStates();
    if (event->pointCount()==3 && event->touchPointStates()==QEventPoint::Pressed) {
        setCursor(Qt::ClosedHandCursor);
    }
}


/*void CustomChartView::paintEvent(QPaintEvent *event) {
    QChartView::paintEvent(event); //不能用CustomChartView
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    //view和chart的尺寸一样
    QBrush brush(QColor(0,0,255, 45));
    // painter.setBrush(brush);
    // painter.drawRect(rect());
    // brush.setColor(QColor(0,255,0, 20));
    // painter.setBrush(brush);
    // painter.drawRect(this->chart()->plotArea());
    qDebug() << "坐标范围:" <<  this->chart()->plotArea();
    // brush.setColor(QColor(255,0,0, 15));
    // painter.setBrush(brush);
    // painter.drawRect(this->chart()->rect());
    QPointF leftBottom = this->chart()->plotArea().bottomLeft();
    QPointF rightTop = this->chart()->plotArea().topRight();
    qDebug() << "坐标范围左下角点:" <<  leftBottom;
    qDebug() << "坐标范围右上角点:" <<  rightTop;
    // brush.setColor(QColor(0,0,0, 25));
    // painter.setBrush(brush);
    // painter.drawRect(this->viewport()->rect());
    // painter.drawLine(leftBottom, rightTop);
    //chart视图坐标系x向右，y向下，chart数据坐标系x向右，y向上
    QPointF temp;
    if (!autoLine.empty()) {//!autoLine.empty()
        QPointF xVertical(autoLine[0].x(), leftBottom.y());
        temp.setX(autoLine[0].x());
        temp.setY(leftBottom.y());
        qDebug() << "temp1:" << temp;
        QPointF yHorizontal(leftBottom.x(), autoLine[0].y());
        QPen pen(QColor(Qt::gray), 2, Qt::DashDotLine);
        painter.setPen(pen);
        painter.drawLine(autoLine[1], autoLine[0]); //autoLine[1] xVertical
        painter.drawLine(autoLine[0], autoLine[2]);//autoLine[2] yHorizontal
        painter.setBrush(brush);
        painter.drawEllipse(QRectF(autoLine[1].x(), autoLine[1].y(), 10,10));
        // qDebug() << "视图下数据点:" <<  autoLine[0];
        qDebug() << "视图下X轴上的点(坐标范围求):" <<  autoLine[1];
        qDebug() << "视图下X轴上的点(绘图区域求):" <<  xVertical;
        // autoLine.clear();
        painter.drawLine(leftBottom, QPointF(0, 0));
        painter.setBrush(brush);
        painter.drawEllipse(QRectF(leftBottom.x(), leftBottom.y(), 10,10));
        painter.drawEllipse(QRectF(leftBottom.x()+100, leftBottom.y(), 10,10));
    }

    qDebug() << "temp2:" << temp;
    painter.drawLine(leftBottom, temp);
    // painter.drawLine(leftBottom, QPointF(0, 0));


}*/

//事件总是触发不了
// void CustomChartView::keyPressEvent(QKeyEvent *event) {
//     if (event->key() == Qt::Key_Control) {
//         emit signalKeyEvent(0, event);
//     }
//     QChartView::keyPressEvent(event);
// }
//
// void CustomChartView::keyReleaseEvent(QKeyEvent *event) {
//     if (event->key() == Qt::Key_Control) {
//         emit signalKeyEvent(1, event);
//     }
//     QChartView::keyReleaseEvent(event);
// }



/*void CustomChartView::setCurSeries(QLineSeries *series) {
    curSeries = series;
}*/

/*
void CustomChartView::hoverMoveEvent(QMouseEvent *event) {
    if (isVisible) {
        // 将鼠标的场景坐标转换为图表坐标
        qDebug() << "场景坐标：" << event->pos();
        QPointF chartCoords = chart()->mapToValue(event->pos());
        qDebug() << "chart坐标坐标：" << chartCoords;
        // 获取数据序列
        QLineSeries *series = curSeries;//static_cast<QLineSeries *>(chart()->series().at(0));
        // 寻找最接近鼠标位置的数据点
        QPointF closestPoint;
        qreal minDist = std::numeric_limits<qreal>::max();
        for (const QPointF &point : series->points()) {
            qreal dist = qSqrt(qPow(point.x() - chartCoords.x(), 2) + qPow(point.y() - chartCoords.y(), 2));
            if (dist < minDist) {
                minDist = dist;
                closestPoint = point;
            }
        }
        // 显示包含数据点坐标的提示信息
        if (formatFlag) {
            QToolTip::showText(event->pos(), QString("Y: %2").arg(closestPoint.y()), this);
        } else {
            QToolTip::showText(event->pos(), QString("X: %2\nY: %2").arg(closestPoint.x()).arg(closestPoint.y()), this);
        }
    }
}
*/
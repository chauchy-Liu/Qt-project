//
// Created by chauchy on 25-7-9.
//

#ifndef CUSTOMCHARTVIEW_H
#define CUSTOMCHARTVIEW_H

#include <QtCharts>
#include <QChartView>
#include <QToolTip>
#include <QGraphicsSceneHoverEvent>

// using namespace QtCharts;
// Qt_CHARTS_USE_NAMESPACE
QT_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_END_NAMESPACE

class CustomChartView: public QChartView {
    Q_OBJECT
public:
    CustomChartView(QWidget *parent = 0);
    ~CustomChartView();
    QList<QPointF> autoLine; //辅助线
    bool hoverState = false;
    QPointF beginPoint;
    QPointF endPoint;
    bool controlPressed = false;
    // bool isVisible;
    // bool formatFlag;
    // void setCurSeries(QLineSeries* series);
    void grabGestures();

private:
    qreal m_currentStepScaleFactor{ 1 };   //一次手势之后缩放的比例
    qreal m_scaleFactor{ 1 };   //缩放比例
    QList<QPointF> zoomCursorView;
    QList<QPointF> zoomCursorChart;

protected:
    // void hoverMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    // void paintEvent(QPaintEvent *event) override;
    /**
     * @brief 事件分发器
     * @param event
     * @return
     */
    bool event(QEvent *event) override;

    /**
     * @brief 事件处理
     * @param event
     * @return
     */
    bool gestureEvent(QGestureEvent *event);

    void tapHoldTriggered(QTapAndHoldGesture *tapAndHold);

    /**
     * @brief 滑动
     * @param gesture
     */
    void panTriggered(QPanGesture* gesture);

    /**
     * @brief 捏合
     * @param gesture
     */
    void pinchTriggered(QPinchGesture* gesture);

    /**
     * @brief 平移
     * @param gesture
     */
    void swipeTriggered(QSwipeGesture* gesture);
    void touchBeginEvent(QTouchEvent *event);
    void touchUpdateEvent(QTouchEvent *event);


signals:
    void signalMouseEvent(int eventId, QMouseEvent *event);
    void signalWheelEvent(QWheelEvent *event);
    // void signalKeyEvent(int eventId, QKeyEvent *event);

};



#endif //CUSTOMCHARTVIEW_H

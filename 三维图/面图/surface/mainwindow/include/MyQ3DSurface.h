//
// Created by chauchy on 25-7-21.
//

#ifndef MYQ3DBARS_H
#define MYQ3DBARS_H
#include <Q3DSurface>



class MyQ3DSurface: public Q3DSurface {
    Q_OBJECT
public:
    MyQ3DSurface(const QSurfaceFormat *format = nullptr, QWindow *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

signals:
    void mousePress(QMouseEvent* event);
    void mouseMove(QMouseEvent* event);
    void mouseRelease(QMouseEvent* event);
    void wheel(QWheelEvent* event);

};



#endif //MYQ3DBARS_H

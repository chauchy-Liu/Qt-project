//
// Created by chauchy on 25-7-2.
//

#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H
#include <QApplication>
#include <QGraphicsView>


class MyGraphicsView: public QGraphicsView {
    Q_OBJECT
public:
    MyGraphicsView(QWidget *parent = 0);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
signals:
    void mouseMovePoint(QPoint point);
    void mouseClicked(QPoint point);
private slots:



};



#endif //MYGRAPHICSVIEW_H

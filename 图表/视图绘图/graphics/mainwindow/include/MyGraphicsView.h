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
    /**
     * @brief
     * @param parent
     */
    explicit MyGraphicsView(QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
signals:
    void mouseMovePoint(QPoint point);
    void mouseClicked(QPoint point);
    void keyPressed(QKeyEvent *event);
    void mouseDoubleClicked(QPoint event);
private slots:



};



#endif //MYGRAPHICSVIEW_H

//
// Created by chauchy on 25-7-1.
//

#ifndef MYBUTTON_H
#define MYBUTTON_H
#include <QWidget>
#include <QMouseEvent>
#include <QPropertyAnimation>


class MyButton: public QWidget {
    Q_OBJECT
public:
    MyButton(QWidget* parent);
    ~MyButton();
protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void resizeEvent(QResizeEvent*) override;
private:
    bool isOff = true;
    QBrush offBgBrush = QBrush(Qt::black);
    QBrush offFgBrush = QBrush(Qt::red);
    QColor offRectColor = QColor(Qt::black);
    QString offText = "OFF";
    QBrush onBgBrush = QBrush(Qt::gray);
    QBrush onFgBrush = QBrush(Qt::green);
    QColor onRectColor = QColor(Qt::gray);
    QString onText = "ON";
    QPropertyAnimation* animation;
    int currentX = height()/2;
signals:
    void isClicked();
    void isClickedWithParam(bool);

};



#endif //MYBUTTON_H

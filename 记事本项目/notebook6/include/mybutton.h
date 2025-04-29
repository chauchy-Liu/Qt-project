#ifndef MYBUTTON_H
#define MYBUTTON_H
#include <QWidget>

class MyButton: public QWidget{
    Q_OBJECT
private:
    QPixmap pic;
public:
    explicit MyButton(QWidget *parent = nullptr);
    // ~MyButton();

    //
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    //绘图事件，update()之后会调用
    void paintEvent(QPaintEvent* event) override;

signals:
    void clicked();

};


#endif // MYBUTTON_H

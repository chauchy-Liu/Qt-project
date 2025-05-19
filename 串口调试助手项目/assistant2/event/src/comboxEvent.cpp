#include "comboxEvent.h"

ComboxEvent::ComboxEvent(QWidget *parent): QComboBox(parent)
{

}

void ComboxEvent::mousePressEvent(QMouseEvent *event)
{
    //发出信号
    if (event->button() == Qt::LeftButton){
        emit refresh();
    }
    QComboBox::mousePressEvent(event);
}

//
// Created by chauchy on 25-5-21.
//

#include "socketComboBox.h"
#include <QMouseEvent>


SocketComboBox::SocketComboBox(QWidget* parent): QComboBox(parent) {

}

void SocketComboBox::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit on_ComboBox_clicked();
    }
    //向父类传递该事件
    QComboBox::mousePressEvent(event);
}

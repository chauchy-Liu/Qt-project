#include "myTextEdit.h"
#include <QWheelEvent>

MyTextEdit::MyTextEdit(QWidget *parent): QTextEdit(parent)
{

}

void MyTextEdit::wheelEvent(QWheelEvent *event)
{
    qDebug() << "MyTextEdit.cpp wheelEvent";
    qDebug() << event->angleDelta();
    if (event->angleDelta().y() > 0 && this->ctrlKeyPressed) {
        // Zoom in
        qDebug() << "Zoom In";
        this->zoomIn();
        event->accept();
    } else if (event->angleDelta().y() < 0 && this->ctrlKeyPressed) {
        // Zoom out
        qDebug() << "Zoom Out";
        this->zoomOut();
        event->accept();
    } else {
        // 其他事件交给父类处理
        QTextEdit::wheelEvent(event);
    }//end if 

}

void MyTextEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control){
        qDebug() << "Ctrl Key Pressed";
        this->ctrlKeyPressed = true;
        event->accept();
    }
    else {
        // 其他按键交给父类处理
        QTextEdit::keyPressEvent(event);
    }

}

void MyTextEdit::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control){
        qDebug() << "Ctrl Key Released";
        this->ctrlKeyPressed = false;
        event->accept();
    }
    else {
        // 其他按键交给父类处理
        QTextEdit::keyPressEvent(event);
    }
}

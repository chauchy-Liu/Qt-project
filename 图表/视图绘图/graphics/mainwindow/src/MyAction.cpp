//
// Created by chauchy on 25-7-4.
//

#include "MyAction.h"
#include <QEvent>
#include <QMouseEvent>

MyAction::MyAction():QAction() {
}

MyAction::~MyAction() {
}

bool MyAction::event(QEvent *e) {
    if (e->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent *>(e);
        if (mouseEvent->button() == Qt::LeftButton) {
            emit clicked();
            return true;
        }
    }
    return QAction::event(e);
}


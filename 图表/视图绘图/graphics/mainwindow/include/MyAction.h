//
// Created by chauchy on 25-7-4.
//

#ifndef MYACTION_H
#define MYACTION_H
#include <QAction>


class MyAction: public QAction {
    Q_OBJECT
public:
    MyAction();
    ~MyAction();
protected:
    bool event(QEvent* e) override;
signals:
    void clicked();

};



#endif //MYACTION_H

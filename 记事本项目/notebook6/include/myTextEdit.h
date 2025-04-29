#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QTextEdit>

class MyTextEdit: public QTextEdit
{
    Q_OBJECT
public: 
    MyTextEdit(QWidget *parent);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    bool ctrlKeyPressed = false;
};

#endif // MYTEXTEDIT_H
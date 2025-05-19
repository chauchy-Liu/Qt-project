#ifndef COMBOXEVENT_H
#define COMBOXEVENT_H
#include <QComboBox>
#include <QWidget>
#include <QMouseEvent>

class ComboxEvent: public QComboBox
{
    Q_OBJECT
public:
    ComboxEvent(QWidget* parent);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    
signals:
    void refresh();
};

#endif // COMBOXEVENT_H
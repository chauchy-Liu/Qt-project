#ifndef CUSTOMTHREAD_H
#define CUSTOMTHREAD_H
#include <QThread>
#include <QWidget>

class CustomThread : public QThread
{
    Q_OBJECT
public:
    CustomThread(QWidget* parent);
protected:
    void run() override;

signals:
    void threadTimeOut();
};

#endif // CUSTOMTHREAD_H
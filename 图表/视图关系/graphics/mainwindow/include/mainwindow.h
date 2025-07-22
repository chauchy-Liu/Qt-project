#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include "MyGraphicsView.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel* labViewCord;
    QLabel* labSceneCord;
    QLabel* labItemCord;
    QGraphicsScene* scene;
    QList<QGraphicsItem*> manageHeap;

    /**
     * @brief 画图元
     */
    void initGraphics();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void on_mouseMovePoint(QPoint pos);
    void on_mouseClicked(QPoint pos);
};
#endif // MAINWINDOW_H

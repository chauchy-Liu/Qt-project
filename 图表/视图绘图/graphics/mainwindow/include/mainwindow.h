#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include "MyGraphicsView.h"
#include <QSplitter>
#include <QKeyEvent>

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
    QLabel* itemInfo;
    QGraphicsScene* scene;
    QList<QGraphicsItem*> manageHeap;
    QFrame* splitters = new QFrame[3];
    int itemID = 1;
    int itemDesc = 2;
    // QString itemDesc = QString();
    int seqNum = 0;


    /**
     * @brief 画图元
     */
    void initGraphics();

protected:
    void resizeEvent(QResizeEvent *event) override;
    // void paintEvent(QPaintEvent *event);

private slots:
    void on_mouseMovePoint(QPoint pos);
    void on_mouseClicked(QPoint pos);
    void on_keyPressed(QKeyEvent *event);
    void on_mouseDoubleClicked(QPoint pos);

    void on_quitActionTriggered();
    void on_rectangleActionTriggered();
    void on_ellipseActionTriggered();
    void on_circleActionTriggered();
    void on_triangleActionTriggered();
    void on_trapezoidActionTriggered();
    void on_lineActionTriggered();
    void on_textActionTriggered();
    void on_bigActionTriggered();
    void on_smallActionTriggered();
    void on_restoreActionTriggered();
    void on_leftActionTriggered();
    void on_rightActionTriggered();
    void on_forwardActionTriggered();
    void on_backwardActionTriggered();
    void on_combineActionTriggered();
    void on_divorceActionTriggered();
    void on_deleteActionTriggered();
};
#endif // MAINWINDOW_H

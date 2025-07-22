#include "mainwindow.h"

#include <QApplication>

#include "ui_mainwindow.h"
// #include <QRectF>
#include <QGraphicsRectItem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setLayout(ui->wholeVL);
    //################################
    labViewCord = new QLabel("View坐标：");
    labViewCord->setMinimumWidth(150);
    ui->statusbar->addWidget(labViewCord);
    //##############################
    labSceneCord = new QLabel("Scene坐标：");
    labSceneCord->setMinimumWidth(150);
    ui->statusbar->addWidget(labSceneCord);
    //##############################
    labItemCord = new QLabel("Item坐标：");
    labItemCord->setMinimumWidth(150);
    ui->statusbar->addWidget(labItemCord);
    //光标形状
    ui->view->setCursor(Qt::CrossCursor);
    //光标跟踪
    ui->view->setMouseTracking(true);
    //画图元
    initGraphics();
    //
    ui->labSceneRect->setText(QString::asprintf("Graphices View坐标，左上角总是（0，0），宽度=%d，高度=%d", ui->view->rect().width(), ui->view->rect().height()));
    //信号与槽
    connect(ui->view, &MyGraphicsView::mouseMovePoint, this, &MainWindow::on_mouseMovePoint);
    connect(ui->view, &MyGraphicsView::mouseClicked, this, &MainWindow::on_mouseClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    //清空指针指向的对象
    for (QGraphicsItem* i: manageHeap) {
        delete i;
    }
    //清空指针
    manageHeap.clear();
}

void MainWindow::initGraphics() {
    //绘图区域
    QRectF rect(-200., -100, 400, 200);
    //建场景
    scene = new QGraphicsScene(rect);
    //创建图元区域
    QGraphicsRectItem* item = new QGraphicsRectItem(rect);
    manageHeap.append(item);
    //场景中添加图元
    scene->addItem(item);
    //视图中再添加场景
    ui->view->setScene(scene);
    //图元可选中、可聚焦
    item->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    QPen pen;
    pen.setWidth(2);
    //图元画笔设置
    item->setPen(pen);
    //###############################################
    //圆图元
    QGraphicsEllipseItem* itemEllipse = new QGraphicsEllipseItem(-100, 50, 200, 100);
    manageHeap.append(itemEllipse);
    itemEllipse->setBrush(QBrush(Qt::blue));
    scene->addItem(itemEllipse);
    QGraphicsEllipseItem* itemEllipse2 = new QGraphicsEllipseItem(-100, -50, 100, 100);
    qDebug() << "创建圆形图元时图元左上角在scene坐标系下位置（-100，-50），其item.pos()：" << itemEllipse2->pos();
    manageHeap.append(itemEllipse2);
    itemEllipse2->setBrush(QBrush(Qt::red));
    scene->addItem(itemEllipse2);
    //移动item中心点到新位置
    itemEllipse2->setPos(rect.right(), rect.bottom());
    qDebug() << QString("设置圆形图元位置(%1,%2)， ").arg(rect.right()).arg(rect.bottom()) << "图元新位置item.pos()："<< itemEllipse2->pos();
    itemEllipse->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    itemEllipse2->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    ui->labSceneRect->setText(QString::asprintf("Graphices View坐标，左上角总是（0，0），宽度=%d，高度=%d", ui->view->rect().width(), ui->view->rect().height()));
    QMainWindow::resizeEvent(event);
}

void MainWindow::on_mouseMovePoint(QPoint pos) {
    labViewCord->setText(QString::asprintf("View坐标：(%d,%d)", pos.x(), pos.y()));
    QPointF scenePos = ui->view->mapToScene(pos);
    labSceneCord->setText(QString::asprintf("Scene坐标：(%.0f,%.0f)", scenePos.x(), scenePos.y()));
    // QPointF itemPos = ui->;
    // labViewCord->setText(QString::asprintf("Item坐标：(%.0f,%.0f)", pos.x(), pos.y()));
    //点击位置收否有图元
    QGraphicsItem* item = NULL;
    item = scene->itemAt(scenePos, ui->view->transform());
    if (item != NULL) {
        // QPointF itemPos = item->pos();
        //场景坐标系转到图元坐标系
        QPointF itemPosInScene = item->mapFromScene(scenePos);
        labItemCord->setText(QString::asprintf("Item坐标(%.0f,%.0f)", itemPosInScene.x(), itemPosInScene.y()));

    } else {
        labItemCord->setText("Item坐标：");
    }
}

void MainWindow::on_mouseClicked(QPoint pos) {
    QPointF scenePos = ui->view->mapToScene(pos);
    //点击位置收否有图元
    QGraphicsItem* item = NULL;
    item = scene->itemAt(scenePos, ui->view->transform());
    if (item != NULL) {
        // QPointF itemPos = item->pos();
        //场景坐标系转到图元坐标系
        QPointF itemPosInScene = item->mapFromScene(scenePos);
        labItemCord->setText(QString::asprintf("Item坐标(%.0f,%.0f)", itemPosInScene.x(), itemPosInScene.y()));

    }  else {
        labItemCord->setText("Item坐标：");
    }

}

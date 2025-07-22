#include "mainwindow.h"

#include <QApplication>

#include "ui_mainwindow.h"
// #include <QRectF>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QDebug>
#include <QLabel>
#include <QTime>
#include <QInputDialog>
#include <QColorDialog>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //布局
    setCentralWidget(ui->view);
    itemInfo = new QLabel("ItemInfo");
    itemInfo->setMinimumWidth(150);
    // itemInfo->setAlignment(Qt::AlignCenter);
    //**********************************************
    //坐标
    //################################
    labViewCord = new QLabel("View坐标：");
    labViewCord->setMinimumWidth(150);
    // labViewCord->setAlignment(Qt::AlignCenter);
    //##############################
    labSceneCord = new QLabel("Scene坐标：");
    labSceneCord->setMinimumWidth(150);
    // labSceneCord->setAlignment(Qt::AlignCenter);
    //##############################
    labItemCord = new QLabel("Item坐标：");
    labItemCord->setMinimumWidth(150);
    // labItemCord->setAlignment(Qt::AlignCenter);
    //组装视图、场景
    scene = new QGraphicsScene(ui->view);
    ui->view->setScene(scene);
    //工具栏
    //状态栏, 带分隔符
    for (int i=0; i<3; i++) {
        splitters[i].setFrameShape(QFrame::VLine);
        splitters[i].setFrameShadow(QFrame::Sunken);
        splitters[i].setStyleSheet("QFrame {color:gray; margin-left: 3px; margin-right: 3px;}");
    }
    ui->statusbar->addWidget(labViewCord, 1);
    ui->statusbar->addWidget(&splitters[0]);
    ui->statusbar->addWidget(labSceneCord, 1);
    ui->statusbar->addWidget(&splitters[1]);
    ui->statusbar->addWidget(labItemCord, 1);
    ui->statusbar->addWidget(&splitters[2]);
    ui->statusbar->addWidget(itemInfo, 1);
    //光标形状
    ui->view->setCursor(Qt::CrossCursor);
    //光标跟踪
    ui->view->setMouseTracking(true);
    //区域选择
    ui->view->setDragMode(QGraphicsView::RubberBandDrag);
    // //画图元
    // initGraphics();
    //随机放置图元
    qsrand(QTime::currentTime().second());
    //信号与槽
    //自定义MyGraphicsView信号
    connect(ui->view, &MyGraphicsView::mouseMovePoint, this, &MainWindow::on_mouseMovePoint);
    connect(ui->view, &MyGraphicsView::mouseClicked, this, &MainWindow::on_mouseClicked);
    connect(ui->view, &MyGraphicsView::keyPressed, this, &MainWindow::on_keyPressed);
    connect(ui->view, &MyGraphicsView::mouseDoubleClicked, this, &MainWindow::on_mouseDoubleClicked);
    //退出系统
    connect(ui->quitAction, &QAction::triggered, this, &MainWindow::on_quitActionTriggered);
    //添加矩形图元
    connect(ui->rectangleAction, &QAction::triggered, this, &MainWindow::on_rectangleActionTriggered);
    connect(ui->ellipseAction, &QAction::triggered, this, &MainWindow::on_ellipseActionTriggered);
    connect(ui->circleAction, &QAction::triggered, this, &MainWindow::on_circleActionTriggered);
    connect(ui->triangleAction, &QAction::triggered, this, &MainWindow::on_triangleActionTriggered);
    connect(ui->trapezoidAction, &QAction::triggered, this, &MainWindow::on_trapezoidActionTriggered);
    connect(ui->lineAction, &QAction::triggered, this, &MainWindow::on_lineActionTriggered);
    connect(ui->textAction, &QAction::triggered, this, &MainWindow::on_textActionTriggered);
    connect(ui->bigAction, &QAction::triggered, this, &MainWindow::on_bigActionTriggered);
    connect(ui->smallAction, &QAction::triggered, this, &MainWindow::on_smallActionTriggered);
    connect(ui->restoreAction, &QAction::triggered, this, &MainWindow::on_restoreActionTriggered);
    connect(ui->leftAction, &QAction::triggered, this, &MainWindow::on_leftActionTriggered);
    connect(ui->rightAction, &QAction::triggered, this, &MainWindow::on_rightActionTriggered);
    connect(ui->forwardAction, &QAction::triggered, this, &MainWindow::on_forwardActionTriggered);
    connect(ui->backwardAction, &QAction::triggered, this, &MainWindow::on_backwardActionTriggered);
    connect(ui->combineAction, &QAction::triggered, this, &MainWindow::on_combineActionTriggered);
    connect(ui->divorceAction, &QAction::triggered, this, &MainWindow::on_divorceActionTriggered);
    connect(ui->deleteAction, &QAction::triggered, this, &MainWindow::on_deleteActionTriggered);


}

MainWindow::~MainWindow()
{
    delete ui;
    //清空指针指向的对象
    for (QGraphicsItem* i: manageHeap) {
        delete i;
    }
    delete[] splitters;
    //清空指针
    manageHeap.clear();
}

void MainWindow::initGraphics() {
    // //绘图区域
    // QRectF rect(-200., -100, 400, 200);
    // //建场景
    // scene = new QGraphicsScene(rect);
    //创建图元区域
    // QGraphicsRectItem* item = new QGraphicsRectItem(rect);
    // manageHeap.append(item);
    //场景中添加图元
    // scene->addItem(item);
    // //视图中再添加场景
    // ui->view->setScene(scene);
    //图元可选中、可聚焦
    // item->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    QPen pen;
    pen.setWidth(2);
    //图元画笔设置
    // item->setPen(pen);
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
    // //移动item中心点到新位置
    // itemEllipse2->setPos(rect.right(), rect.bottom());
    // qDebug() << QString("设置圆形图元位置(%1,%2)， ").arg(rect.right()).arg(rect.bottom()) << "图元新位置item.pos()："<< itemEllipse2->pos();
    itemEllipse->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    itemEllipse2->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    // update();

}

void MainWindow::resizeEvent(QResizeEvent *event) {
    // update();
    QMainWindow::resizeEvent(event);
}

// void MainWindow::paintEvent(QPaintEvent *event) {
//     QGraphicsRectItem* item = dynamic_cast<QGraphicsRectItem*>(manageHeap[0]);
//     QGraphicsEllipseItem* itemEllipse = dynamic_cast<QGraphicsEllipseItem*>(manageHeap[1]);
//     QGraphicsEllipseItem* itemEllipse2 = dynamic_cast<QGraphicsEllipseItem*>(manageHeap[2]);
//     //画图元的原点
//     int originSize = 60;
//     // QPainter painter(ui->view);
//     QPainter painter(this);
//     QPen pen(QColor(Qt::gray), 4);
//     painter.setPen(pen);
//     painter.setBrush(Qt::white);
//     painter.drawEllipse(ui->view->mapFromScene(item->pos()).x()-originSize, ui->view->mapFromScene(item->pos()).y()-originSize, 2*originSize, 2*originSize);
//     //#####################
//     painter.setBrush(QColor(Qt::blue));
//     painter.drawEllipse(ui->view->mapFromScene(itemEllipse->pos()).x()-originSize, ui->view->mapFromScene(itemEllipse->pos()).y()-originSize, 2*originSize, 2*originSize);
//     //#####################
//     painter.setBrush(QColor(Qt::red));
//     painter.drawEllipse(ui->view->mapFromScene(itemEllipse2->pos()).x()-originSize, ui->view->mapFromScene(itemEllipse2->pos()).y()-originSize, 2*originSize, 2*originSize);
//     QMainWindow::paintEvent(event);
// }

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
    //点击位置是否有图元
    QGraphicsItem* item = NULL;
    item = scene->itemAt(scenePos, ui->view->transform());
    if (item != NULL) {
        // QPointF itemPos = item->pos();
        //场景坐标系转到图元坐标系
        QPointF itemPosInScene = item->mapFromScene(scenePos);
        labItemCord->setText(QString::asprintf("Item坐标(%.0f,%.0f)", itemPosInScene.x(), itemPosInScene.y()));
        itemInfo->setText(QString("Item形状: %1, ItemID: %2").arg(item->data(itemDesc).toString()).arg(item->data(itemID).toInt()));
        // update();
    }  else {
        labItemCord->setText("Item坐标：");
    }

}

void MainWindow::on_keyPressed(QKeyEvent *event) {
    qDebug() << "key: " << event->key();
    int cnt = scene->selectedItems().count();
    if (cnt == 0) {return;}
    switch (event->key()) {
        case Qt::Key_Backspace: {
            on_deleteActionTriggered();
            break;
        }
        case Qt::Key_Left: {
            for (int i = 0; i < cnt; i++) {
                QGraphicsItem* item = scene->selectedItems().at(i);
                item->setX(item->x()-1);
            }
            break;
        }
        case Qt::Key_Right: {
            for (int i = 0; i < cnt; i++) {
                QGraphicsItem* item = scene->selectedItems().at(i);
                item->setX(item->x()+1);
            }
            break;
        }
        case Qt::Key_Up: {
            for (int i = 0; i < cnt; i++) {
                QGraphicsItem* item = scene->selectedItems().at(i);
                item->setY(item->y()-1);
            }
            break;
        }
        case Qt::Key_Down: {
            for (int i = 0; i < cnt; i++) {
                QGraphicsItem* item = scene->selectedItems().at(i);
                item->setY(item->y()+1);
            }
            break;
        }
    }
}

void MainWindow::on_mouseDoubleClicked(QPoint pos) {
    QPointF scenePos = ui->view->mapToScene(pos);
    //点击位置是否有图元
    QGraphicsItem* item = NULL;
    item = scene->itemAt(scenePos, ui->view->transform());
    if (item != NULL) {
        switch (item->type()) {
            case QGraphicsRectItem::Type: {
                QGraphicsRectItem* theItem;
                theItem = qgraphicsitem_cast<QGraphicsRectItem*>(item);
                QColor color = theItem->brush().color();
                // qDebug() << "改颜色前"<< color;
                color = QColorDialog::getColor(color, NULL, "选择颜色");
                if (color.isValid()) {
                    // qDebug() << "改颜色后" << color;
                    theItem->setBrush(QBrush(color));
                }
                break;
            }
            case QGraphicsEllipseItem::Type: {
                QGraphicsEllipseItem* theItem;
                theItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
                QColor color = theItem->brush().color();
                // qDebug() << "改颜色前"<< color;
                color = QColorDialog::getColor(color, NULL, "选择颜色");
                if (color.isValid()) {
                    // qDebug() << "改颜色后" << color;
                    theItem->setBrush(QBrush(color));
                }
                break;
            }
            case QGraphicsPolygonItem::Type: {
                QGraphicsPolygonItem* theItem;
                theItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
                QColor color = theItem->brush().color();
                // qDebug() << "改颜色前"<< color;
                color = QColorDialog::getColor(color, NULL, "选择颜色");
                if (color.isValid()) {
                    // qDebug() << "改颜色后" << color;
                    theItem->setBrush(QBrush(color));
                }
                break;
            }
            case QGraphicsSimpleTextItem::Type: {
                QGraphicsSimpleTextItem* theItem;
                theItem = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
                QColor color = theItem->pen().color();
                // qDebug() << "改颜色前"<< color;
                if (color.isValid()) {
                    color = QColorDialog::getColor(color, NULL, "选择颜色");
                    // qDebug() << "改颜色后" << color;
                    theItem->setPen(QPen(color));
                }
                break;
            }
            case QGraphicsLineItem::Type: {
                QGraphicsLineItem* theItem;
                theItem = qgraphicsitem_cast<QGraphicsLineItem*>(item);
                QColor color = theItem->pen().color();
                // qDebug() << "改颜色前"<< color;
                int width = theItem->pen().width();
                color = QColorDialog::getColor(color, NULL, "选择颜色");
                if (color.isValid()) {
                    // qDebug() << "改颜色后" << color;
                    theItem->setPen(QPen(color, width));
                }
                break;
            }
        }

    } else {
        return;
    }
}

void MainWindow::on_quitActionTriggered() {
    this->close();
}

void MainWindow::on_rectangleActionTriggered() {
    QGraphicsRectItem* item = new QGraphicsRectItem(-50,-30, 100, 60);
    manageHeap.append(item);
    item->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    item->setBrush(QBrush(Qt::yellow));
    //设置创建位置
    QPoint viewPos;
    viewPos.setX(25);
    viewPos.setY(15);
    item->setPos(ui->view->mapToScene(ui->view->pos().x()+qrand()%(ui->view->rect().width()-50), ui->view->pos().y()+qrand()%(ui->view->rect().height()-30)));
    //设置id和形状
    item->setData(itemID, ++seqNum);
    item->setData(itemDesc, "矩形");

    scene->addItem(item);
    //将场景中的过往选中状态取消
    scene->clearSelection();
    item->setSelected(true);
}

void MainWindow::on_ellipseActionTriggered() {
    QGraphicsEllipseItem* item = new QGraphicsEllipseItem(-50,-30, 100, 60);
    manageHeap.append(item);
    item->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    item->setBrush(QBrush(Qt::gray));
    //设置创建位置
    QPoint viewPos;
    viewPos.setX(25);
    viewPos.setY(15);
    item->setPos(ui->view->mapToScene(ui->view->pos().x()+qrand()%(ui->view->rect().width()-50), ui->view->pos().y()+qrand()%(ui->view->rect().height()-30)));
    //设置id和形状
    item->setData(itemID, ++seqNum);
    item->setData(itemDesc, "椭圆形");

    scene->addItem(item);
    //将场景中的过往选中状态取消
    scene->clearSelection();
    item->setSelected(true);
}

void MainWindow::on_circleActionTriggered() {
    QGraphicsEllipseItem* item = new QGraphicsEllipseItem(-50,-50, 100, 100);
    manageHeap.append(item);
    item->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    item->setBrush(QBrush(Qt::blue));
    //设置创建位置
    QPoint viewPos;
    viewPos.setX(25);
    viewPos.setY(25);
    item->setPos(ui->view->mapToScene(ui->view->pos().x()+qrand()%(ui->view->rect().width()-50), ui->view->pos().y()+qrand()%(ui->view->rect().height()-50)));
    //设置id和形状
    item->setData(itemID, ++seqNum);
    item->setData(itemDesc, "圆形");

    scene->addItem(item);
    //将场景中的过往选中状态取消
    scene->clearSelection();
    item->setSelected(true);
}

void MainWindow::on_triangleActionTriggered() {
    QGraphicsPolygonItem* item = new QGraphicsPolygonItem;
    QPolygonF points;
    points.append(QPointF(0,-40));
    points.append(QPointF(60,40));
    points.append(QPointF(-60,40));
    item->setPolygon(points);
    manageHeap.append(item);
    item->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    item->setBrush(QBrush(Qt::red));
    //设置创建位置
    QPoint viewPos;
    viewPos.setX(60);
    viewPos.setY(40);
    item->setPos(ui->view->mapToScene(ui->view->pos().x()+qrand()%(ui->view->rect().width()-50), ui->view->pos().y()+qrand()%(ui->view->rect().height()-30)));
    //设置id和形状
    item->setData(itemID, ++seqNum);
    item->setData(itemDesc, "三角形");

    scene->addItem(item);
    //将场景中的过往选中状态取消
    scene->clearSelection();
    item->setSelected(true);
}

void MainWindow::on_trapezoidActionTriggered() {
    QGraphicsPolygonItem* item = new QGraphicsPolygonItem;
    QPolygonF points;
    points.append(QPointF(-40,-40));
    points.append(QPointF(40,-40));
    points.append(QPointF(60,40));
    points.append(QPointF(-60,40));
    item->setPolygon(points);
    manageHeap.append(item);
    item->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    item->setBrush(QBrush(Qt::green));
    //设置创建位置
    QPoint viewPos;
    viewPos.setX(60);
    viewPos.setY(40);
    item->setPos(ui->view->mapToScene(ui->view->pos().x()+qrand()%(ui->view->rect().width()-50), ui->view->pos().y()+qrand()%(ui->view->rect().height()-30)));
    //设置id和形状
    item->setData(itemID, ++seqNum);
    item->setData(itemDesc, "梯形");

    scene->addItem(item);
    //将场景中的过往选中状态取消
    scene->clearSelection();
    item->setSelected(true);
}

void MainWindow::on_lineActionTriggered() {
    QGraphicsLineItem* item = new QGraphicsLineItem(-100,0, 100, 0);
    manageHeap.append(item);
    item->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(3);
    item->setPen(pen);
    //设置创建位置
    QPoint viewPos;
    viewPos.setX(50);
    viewPos.setY(50);
    item->setPos(ui->view->mapToScene(ui->view->pos().x()+qrand()%(ui->view->rect().width()-50), ui->view->pos().y()+qrand()%(ui->view->rect().height()-30)));
    //设置id和形状
    item->setData(itemID, ++seqNum);
    item->setData(itemDesc, "直线");

    scene->addItem(item);
    //将场景中的过往选中状态取消
    scene->clearSelection();
    item->setSelected(true);
}

void MainWindow::on_textActionTriggered() {
    QString str = QInputDialog::getText(this, "输入对话框", "请输入: ");
    QGraphicsSimpleTextItem* item = new QGraphicsSimpleTextItem(str);
    manageHeap.append(item);
    item->setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    QPen pen;
    pen.setColor(Qt::darkGray);
    pen.setWidth(3);
    item->setPen(pen);
    QFont font;
    font.setPointSize(80);
    item->setFont(font);
    //设置创建位置
    QPoint viewPos;
    viewPos.setX(25);
    viewPos.setY(15);
    item->setPos(ui->view->mapToScene(ui->view->pos().x()+qrand()%(ui->view->rect().width()-50), ui->view->pos().y()+qrand()%(ui->view->rect().height()-30)));
    //设置id和形状
    item->setData(itemID, ++seqNum);
    item->setData(itemDesc, "文字");

    scene->addItem(item);
    //将场景中的过往选中状态取消
    scene->clearSelection();
    item->setSelected(true);
}

void MainWindow::on_bigActionTriggered() {
    //查看选中的都有哪些
    int cnt = scene->selectedItems().count();
    if (cnt == 0) {
        ui->view->scale(1.1, 1.1);
        return;
    }
    for (int i = 0; i < scene->selectedItems().count(); i++) {
        QGraphicsItem* item =scene->selectedItems()[i];
        item->setScale(0.1+item->scale());
    }
}

void MainWindow::on_smallActionTriggered() {
    //查看选中的都有哪些
    int cnt = scene->selectedItems().count();
    if (cnt == 0) {
        ui->view->scale(0.9, 0.9);
        return;
    }
    for (int i = 0; i < scene->selectedItems().count(); i++) {
        QGraphicsItem* item =scene->selectedItems()[i];
        item->setScale(-0.1+item->scale());
    }
}

void MainWindow::on_restoreActionTriggered() {
    //查看选中的都有哪些
    int cnt = scene->selectedItems().count();
    if (cnt == 0) {
        ui->view->resetTransform();
        return;
    }
    for (int i = 0; i < scene->selectedItems().count(); i++) {
        QGraphicsItem* item =scene->selectedItems()[i];
        item->setScale(1);
        // item->resetTransform();
    }
}

void MainWindow::on_leftActionTriggered() {
    //查看选中的都有哪些
    int cnt = scene->selectedItems().count();
    if (cnt == 0) {
        ui->view->rotate(-45);
        return;
    }
    for (int i = 0; i < scene->selectedItems().count(); i++) {
        QGraphicsItem* item =scene->selectedItems()[i];
        item->setRotation(item->rotation()-45);
    }
}

void MainWindow::on_rightActionTriggered() {
    //查看选中的都有哪些
    int cnt = scene->selectedItems().count();
    if (cnt == 0) {
        ui->view->rotate(45);
        return;
    }
    for (int i = 0; i < scene->selectedItems().count(); i++) {
        QGraphicsItem* item =scene->selectedItems()[i];
        item->setRotation(item->rotation()+45);
    }
}

void MainWindow::on_forwardActionTriggered() {
    //查看选中的都有哪些
    int cnt = scene->selectedItems().count();
    if (cnt == 0) {
        return;
    }
    for (int i = 0; i < scene->selectedItems().count(); i++) {
        QGraphicsItem* item =scene->selectedItems()[i];
        item->setZValue(item->zValue()+1);
    }
}

void MainWindow::on_backwardActionTriggered() {
    //查看选中的都有哪些
    int cnt = scene->selectedItems().count();
    if (cnt == 0) {
        return;
    }
    for (int i = 0; i < scene->selectedItems().count(); i++) {
        QGraphicsItem* item =scene->selectedItems()[i];
        item->setZValue(item->zValue()-1);
    }
}

void MainWindow::on_combineActionTriggered() {
    //查看选中的都有哪些
    int cnt = scene->selectedItems().count();
    if (cnt <= 1) {
        return;
    }
    QGraphicsItemGroup* group = new QGraphicsItemGroup;
    manageHeap.append(group);
    scene->addItem(group);
    for (int i = 0; i <cnt; i++) {
        QGraphicsItem* item =scene->selectedItems()[0];
        item->setSelected(false);
        group->addToGroup(item);
    }
    group->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
    group->setSelected(true);
}

void MainWindow::on_divorceActionTriggered() {
    //查看选中的都有哪些
    int cnt = scene->selectedItems().count();
    if (cnt < 1) {
        return;
    }

    QList<QGraphicsItemGroup*> groups;
    for (int i = 0; i < scene->selectedItems().count(); i++) {
        QGraphicsItem* item =scene->selectedItems()[i];
        if (item->type() == QGraphicsItemGroup::Type) {
            QGraphicsItemGroup* group = qgraphicsitem_cast<QGraphicsItemGroup*>(item);
            groups.append(group);
        }
    }
    for (int i=0; i<groups.size(); i++) {
        scene->destroyItemGroup(groups[i]);
    }

}

void MainWindow::on_deleteActionTriggered() {
    //查看选中的都有哪些
    int cnt = scene->selectedItems().count();
    if (cnt < 1) {
        return;
    }
    for (int i=0; i<cnt; i++) {
        QGraphicsItem* item = scene->selectedItems().at(0);
        scene->removeItem(item);
    }
}

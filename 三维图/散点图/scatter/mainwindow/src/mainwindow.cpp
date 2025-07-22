#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSplitter>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(ui->groupBox); //添加参与布局的窗口
    // 三维图表
    // barChart = new Q3DBars;
    initGraph3D();
    //创建三维图表视图组件
    container = createWindowContainer(scatterChart);
    splitter->addWidget(container);
    // QWidget* container = qobject_cast<QSplitter*>(centralWidget());
    //把splitter添加进父窗口
    setCentralWidget(splitter);
    // 启用鼠标跟踪
    container->setMouseTracking(true);

    //安装事件过滤器
    container->installEventFilter(this);
    //信号与槽
    connect(scatterChart, &MyQ3DScatter::mouseMove, this, &MainWindow::onMouseMove);
    connect(scatterChart, &MyQ3DScatter::mousePress, this, &MainWindow::onMousePressed);
    connect(scatterChart, &MyQ3DScatter::mouseRelease, this, &MainWindow::onMouseReleased);
    connect(scatterChart, &MyQ3DScatter::wheel, this, &MainWindow::onWheel);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    qDebug() << "mouseMoveEvent: " << event->pos();
    if (isClicked == true) {
        qreal x = event->pos().x() - pressPos.x();
        qreal y = event->pos().y() - pressPos.y();
        scatterChart->scene()->activeCamera()->setXRotation(x);
        scatterChart->scene()->activeCamera()->setYRotation(y);
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    qDebug() << "mousePressEvent: " ;
    if (event->button() == Qt::LeftButton) {
        isClicked = true;
        pressPos = event->pos();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    qDebug() << "mouseReleaseEvent: " ;
    if (event->button() == Qt::LeftButton) {
        isClicked = false;
    }
    QMainWindow::mouseReleaseEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    // QWidget* container = splitter->widget(1);
    if (obj->objectName() == container->objectName() && event->type() == QEvent::KeyPress) {

    } else if (obj->objectName() == container->objectName() && event->type() == QEvent::MouseButtonPress) {
        qDebug() << "mousePressEvent: " ;
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            isClicked = true;
            pressPos = mouseEvent->pos();
            return true;
        }
    } else if (obj->objectName() == container->objectName() && event->type() == QEvent::MouseButtonRelease) {
        qDebug() << "mouseReleaseEvent: " ;
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            isClicked = false;
            // pressPos = mouseEvent->pos();
            return true;
        }
    } else if (obj->objectName() == container->objectName() && event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        qDebug() << "mouseMoveEvent: " << mouseEvent->pos();
        if (isClicked == true) {
            qreal x = mouseEvent->pos().x() - pressPos.x();
            qreal y = mouseEvent->pos().y() - pressPos.y();
            scatterChart->scene()->activeCamera()->setXRotation(x);
            scatterChart->scene()->activeCamera()->setYRotation(y);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::initGraph3D() {
    //分配空间
    scatterChart = new MyQ3DScatter;
    //创建坐标
    QValue3DAxis* axisY = new QValue3DAxis;
    axisY->setTitle("Y轴");
    axisY->setTitleVisible(true);

    QValue3DAxis* axisX = new QValue3DAxis;
    axisX->setTitle("X轴");
    axisX->setTitleVisible(true);

    QValue3DAxis* axisZ = new QValue3DAxis;
    axisZ->setTitle("Z轴");
    axisZ->setTitleVisible(true);
    //图表添加坐标轴
    scatterChart->setAxisY(axisY);
    scatterChart->setAxisX(axisX);
    scatterChart->setAxisZ(axisZ);

    // //坐标标签
    // QStringList rowLabs;
    // QStringList colLabs;
    // rowLabs << "row1" << "row2" << "row3" << "row4";
    // colLabs << "col1" << "col2" << "col3" << "col4" << "col5";
    // axisRow->setLabels(rowLabs);
    // axisCol->setLabels(colLabs);



    //创建序列
    QScatterDataProxy* proxy = new QScatterDataProxy;
    //序列添加代理
    QScatter3DSeries* series = new QScatter3DSeries(proxy);
    //网格块
    series->setMesh(QAbstract3DSeries::MeshCylinder);
    series->setName("scatter");
    series->setItemLabelFormat("(@rowLabel,@colLabel):%.1f");//@valueLabel

    //图表添加序列
    scatterChart->addSeries(series);

    //添加数据
    int N = 41;
    QScatterDataArray* dataArray = new QScatterDataArray;
    //无行和列概念,设置容器大小
    dataArray->resize(N*N);
    QScatterDataItem* dataItem = &(dataArray->first());
    //墨西哥草帽数据
    float x, y, z;
    x = -10;
    for (int i = 0; i < N; i++) {
        z = -10;
        for (int j = 0; j < N; j++) {
           y = qSqrt(x*x + z*z);
           if (y!=0) {
               y = 10 * qSin(y)/y;
           } else {
               y = 10;
           }
           dataItem->setPosition(QVector3D(x, y, z));
           dataItem++;
           z += 0.5;
        }
        x += 0.5;
    }

    //用代理添加数据
    series->dataProxy()->resetArray(dataArray);

    //初始化图表其他属性
    // int xRot = ui->hSpinSlider->value();
    // int yRot = ui->vSpinSlider->value();
    // int zoom = ui->scaleSlider->value();
    // scatterChart->scene()->activeCamera()->setCameraPosition(xRot,yRot,zoom);
    scatterChart->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPreset(1));
    ui->sightComboBox->setCurrentIndex(2);
    ui->hSpinSlider->setValue(scatterChart->scene()->activeCamera()->xRotation());
    ui->vSpinSlider->setValue(scatterChart->scene()->activeCamera()->yRotation());
    ui->scaleSlider->setValue(scatterChart->scene()->activeCamera()->zoomLevel());
    ui->barComboBox->setCurrentText("Cylinder");
    // ui->barComboBox->setCurrentIndex(1);
    ui->selectComboBox->setCurrentIndex(1);
    ui->backgroundCheckBox->setChecked(true);
    ui->gridCheckBox->setChecked(true);
    ui->itemLabelCheckBox->setChecked(true);
    ui->axisLabelCheckBox->setChecked(true);
    QFont font = scatterChart->activeTheme()->font();
    ui->fontSpinBox->setValue(font.pointSize());
    ui->pointDSpinBox->setValue(series->itemSize());
}

void MainWindow::onMousePressed(QMouseEvent *event) {
    qDebug() << "mousePressEvent: " ;
    if (event->button() == Qt::LeftButton) {
        isClicked = true;
        pressPos = event->pos();
    }
    // QMainWindow::mousePressEvent(event);
}

void MainWindow::onMouseReleased(QMouseEvent *event) {
    qDebug() << "mouseReleaseEvent: " ;
    if (event->button() == Qt::LeftButton) {
        isClicked = false;
    }
    // QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::onMouseMove(QMouseEvent *event) {
    qDebug() << "mouseMoveEvent: " << event->pos();
    if (isClicked == true) {
        qreal x = event->pos().x() - pressPos.x();
        qreal y = event->pos().y() - pressPos.y();
        // QVector3D position = scatterChart->scene()->activeCamera()->position();
        // qDebug() << "x:" << x << "y:" << y << "position: " << position;
        // position.setX(position.x()+x);//
        // position.setY(position.y()+y);//
        // qDebug() << "position+delta: " << position;
        // scatterChart->scene()->activeCamera()->setPosition(position);
    }
    // QMainWindow::mouseMoveEvent(event);
}

void MainWindow::onWheel(QWheelEvent *event) {
    qDebug() << "onWheelEvent: " << event->angleDelta();
    qreal x = scatterChart->scene()->activeCamera()->xRotation() + event->angleDelta().x();
    qreal y = scatterChart->scene()->activeCamera()->yRotation() + event->angleDelta().y();
    if (x>=-180 && x<=180) {
        scatterChart->scene()->activeCamera()->setXRotation(x);
    }
    if (y>=-90 && y<=90) {
        scatterChart->scene()->activeCamera()->setYRotation(y);
    }
}

void MainWindow::on_sightComboBox_activated(int index) {
    //坐标系视角
    scatterChart->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPreset(index-1));
}

void MainWindow::on_hSpinSlider_valueChanged(int value) {
    Q_UNUSED(value);
    int xRot = ui->hSpinSlider->value();
    int yRot = ui->vSpinSlider->value();
    int zoom = ui->scaleSlider->value();
    //计算机图形学
    scatterChart->scene()->activeCamera()->setCameraPosition(xRot, yRot, zoom);
}

void MainWindow::on_vSpinSlider_valueChanged(int value) {
    on_hSpinSlider_valueChanged(value);
}

void MainWindow::on_scaleSlider_valueChanged(int value) {
    on_hSpinSlider_valueChanged(value);
}

void MainWindow::on_themeComboBox_currentIndexChanged(int index) {
    scatterChart->activeTheme()->setType(Q3DTheme::Theme(index));

}

void MainWindow::on_barComboBox_currentIndexChanged(int index) {
    QScatter3DSeries* series = scatterChart->seriesList().at(0);
    series->setMesh(QAbstract3DSeries::Mesh(index));
}

void MainWindow::on_selectComboBox_currentIndexChanged(int index) {
    if (index == 9) {
        scatterChart->setSelectionMode(QAbstract3DGraph::SelectionFlag(16));
    } else {
        scatterChart->setSelectionMode(QAbstract3DGraph::SelectionFlag(index));
    }
}

void MainWindow::on_fontSpinBox_valueChanged(int value) {
    QFont font = scatterChart->activeTheme()->font();
    font.setPointSize(value);
    scatterChart->activeTheme()->setFont(font);
}

void MainWindow:: on_itemBtn_clicked() {
    QScatter3DSeries* series = scatterChart->seriesList().at(0);
    QColor color = series->baseColor();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        series->setBaseColor(color);
    }
}

void MainWindow::on_backgroundCheckBox_clicked(bool checked) {
    scatterChart->activeTheme()->setBackgroundEnabled(checked);
}

void MainWindow::on_gridCheckBox_clicked(bool checked) {
    scatterChart->activeTheme()->setGridEnabled(checked);
}

void MainWindow::on_smoothCheckBox_clicked(bool checked) {
    QScatter3DSeries* series = scatterChart->seriesList().at(0);
    series->setMeshSmooth(checked);
}

void MainWindow::on_invertShadeCheckBox_clicked(bool checked) {
    scatterChart->setReflection(checked);
}

void MainWindow::on_invertValueCheckBox_clicked(bool checked) {
    QValue3DAxis* axisValue = scatterChart->axisY();
    axisValue->setReversed(checked);
}

void MainWindow::on_itemLabelCheckBox_clicked(bool checked) {
    // scatterChart->activeTheme()->setLabelBorderEnabled(checked);
    QScatter3DSeries* series = scatterChart->seriesList().at(0);
    series->setItemLabelVisible(checked);
}

void MainWindow::axisTitleCheckBox_clicked(bool checked) {
    scatterChart->axisY()->setTitleVisible(checked);
    scatterChart->axisX()->setTitleVisible(checked);
    scatterChart->axisZ()->setTitleVisible(checked);
}

void MainWindow::on_axisLabelCheckBox_clicked(bool checked) {
    scatterChart->activeTheme()->setLabelBackgroundEnabled(checked);
}

void MainWindow::on_pointDSpinBox_valueChanged(double value) {
    QScatter3DSeries* series = scatterChart->seriesList().at(0);
    series->setItemSize(value);
}
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
    container = createWindowContainer(surfaceChart);
    splitter->addWidget(container);
    // QWidget* container = qobject_cast<QSplitter*>(centralWidget());
    //把splitter添加进父窗口
    setCentralWidget(splitter);
    // 启用鼠标跟踪
    container->setMouseTracking(true);

    //安装事件过滤器
    container->installEventFilter(this);
    //信号与槽
    connect(surfaceChart, &MyQ3DSurface::mouseMove, this, &MainWindow::onMouseMove);
    connect(surfaceChart, &MyQ3DSurface::mousePress, this, &MainWindow::onMousePressed);
    connect(surfaceChart, &MyQ3DSurface::mouseRelease, this, &MainWindow::onMouseReleased);
    connect(surfaceChart, &MyQ3DSurface::wheel, this, &MainWindow::onWheel);
    //按钮渐变色
    QLinearGradient gradColor1(0,0,100,0);
    gradColor1.setColorAt(1.0, Qt::black);
    gradColor1.setColorAt(0.67, Qt::blue);
    gradColor1.setColorAt(0.33, Qt::red);
    gradColor1.setColorAt(0.0, Qt::yellow);
    //图片面积
    QPixmap pm(160, 20);
    QPainter painter(&pm);
    painter.setBrush(gradColor1);
    painter.drawRect(0,0,160,20);
    //图片贴到按钮上
    ui->itemGradBtn1->setIcon(QIcon(pm));
    ui->itemGradBtn1->setIconSize(QSize(160,20));
    //*****************************************
    gradColor1.setColorAt(1.0, Qt::darkGreen);
    gradColor1.setColorAt(0.5, Qt::yellow);
    gradColor1.setColorAt(0.2, Qt::red);
    gradColor1.setColorAt(0.0, Qt::darkRed);
    //图片面积
    painter.setBrush(gradColor1);
    painter.drawRect(0,0,160,20);
    //图片贴到按钮上
    ui->itemGradBtn2->setIcon(QIcon(pm));
    ui->itemGradBtn2->setIconSize(QSize(160,20));
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
        surfaceChart->scene()->activeCamera()->setXRotation(x);
        surfaceChart->scene()->activeCamera()->setYRotation(y);
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
            surfaceChart->scene()->activeCamera()->setXRotation(x);
            surfaceChart->scene()->activeCamera()->setYRotation(y);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::initGraph3D() {
    //分配空间
    surfaceChart = new MyQ3DSurface;
    //创建坐标
    QValue3DAxis* axisY = new QValue3DAxis;
    axisY->setTitle("Y轴");
    axisY->setTitleVisible(true);

    QValue3DAxis* axisX = new QValue3DAxis;
    axisX->setTitle("X轴");
    axisX->setTitleVisible(true);
    axisX->setRange(-11,11);

    QValue3DAxis* axisZ = new QValue3DAxis;
    axisZ->setTitle("Z轴");
    axisZ->setTitleVisible(true);
    axisZ->setRange(-11,11);
    //图表添加坐标轴
    surfaceChart->setAxisY(axisY);
    surfaceChart->setAxisX(axisX);
    surfaceChart->setAxisZ(axisZ);

    // //坐标标签
    // QStringList rowLabs;
    // QStringList colLabs;
    // rowLabs << "row1" << "row2" << "row3" << "row4";
    // colLabs << "col1" << "col2" << "col3" << "col4" << "col5";
    // axisRow->setLabels(rowLabs);
    // axisCol->setLabels(colLabs);



    //创建序列和序列代理
    QSurfaceDataProxy* proxy = new QSurfaceDataProxy;
    //序列添加代理
    QSurface3DSeries* series = new QSurface3DSeries(proxy);
    //网格块
    // series->setMesh(QAbstract3DSeries::MeshCylinder);
    series->setName("surface");
    series->setItemLabelFormat("(@xLabel,@zLabel):@yLabel");//@valueLabel
    series->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);
    series->setMeshSmooth(true);

    //图表添加序列
    surfaceChart->addSeries(series);

    //添加数据
    int N = 41;
    QSurfaceDataArray* dataArray = new QSurfaceDataArray;
    //设置行数
    dataArray->reserve(N);
    //墨西哥草帽数据
    float x, y, z;
    x = -10;
    for (int i = 0; i < N; i++) {
        QSurfaceDataRow* dataRow = new QSurfaceDataRow(N); //一行数据，列数N
        z = -10;
        int index = 0;
        for (int j = 0; j < N; j++) {
           y = qSqrt(x*x + z*z);
           if (y!=0) {
               y = 10 * qSin(y)/y;
           } else {
               y = 10;
           }
           (*dataRow)[index++].setPosition(QVector3D(x, y, z));
           z += 0.5;
        }
        x += 0.5;
        *dataArray << dataRow;
    }

    //用代理添加数据
    series->dataProxy()->resetArray(dataArray);

    //初始化图表其他属性
    // int xRot = ui->hSpinSlider->value();
    // int yRot = ui->vSpinSlider->value();
    // int zoom = ui->scaleSlider->value();
    // surfaceChart->scene()->activeCamera()->setCameraPosition(xRot,yRot,zoom);
    surfaceChart->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPreset(1));
    ui->sightComboBox->setCurrentIndex(2);
    ui->hSpinSlider->setValue(surfaceChart->scene()->activeCamera()->xRotation());
    ui->vSpinSlider->setValue(surfaceChart->scene()->activeCamera()->yRotation());
    ui->scaleSlider->setValue(surfaceChart->scene()->activeCamera()->zoomLevel());
    ui->barComboBox->setCurrentText("Cylinder");
    // ui->barComboBox->setCurrentIndex(1);
    ui->selectComboBox->setCurrentIndex(1);
    ui->backgroundCheckBox->setChecked(true);
    ui->gridCheckBox->setChecked(true);
    ui->itemLabelCheckBox->setChecked(true);
    ui->axisLabelCheckBox->setChecked(true);
    QFont font = surfaceChart->activeTheme()->font();
    ui->fontSpinBox->setValue(font.pointSize());
    // ui->pointDSpinBox->setValue(series->itemSize());
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
        // QVector3D position = surfaceChart->scene()->activeCamera()->position();
        // qDebug() << "x:" << x << "y:" << y << "position: " << position;
        // position.setX(position.x()+x);//
        // position.setY(position.y()+y);//
        // qDebug() << "position+delta: " << position;
        // surfaceChart->scene()->activeCamera()->setPosition(position);
    }
    // QMainWindow::mouseMoveEvent(event);
}

void MainWindow::onWheel(QWheelEvent *event) {
    qDebug() << "onWheelEvent: " << event->angleDelta();
    qreal x = surfaceChart->scene()->activeCamera()->xRotation() + event->angleDelta().x();
    qreal y = surfaceChart->scene()->activeCamera()->yRotation() + event->angleDelta().y();
    if (x>=-180 && x<=180) {
        surfaceChart->scene()->activeCamera()->setXRotation(x);
    }
    if (y>=-90 && y<=90) {
        surfaceChart->scene()->activeCamera()->setYRotation(y);
    }
}

void MainWindow::on_sightComboBox_activated(int index) {
    //坐标系视角
    surfaceChart->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPreset(index-1));
}

void MainWindow::on_hSpinSlider_valueChanged(int value) {
    Q_UNUSED(value);
    int xRot = ui->hSpinSlider->value();
    int yRot = ui->vSpinSlider->value();
    int zoom = ui->scaleSlider->value();
    //计算机图形学
    surfaceChart->scene()->activeCamera()->setCameraPosition(xRot, yRot, zoom);
}

void MainWindow::on_vSpinSlider_valueChanged(int value) {
    on_hSpinSlider_valueChanged(value);
}

void MainWindow::on_scaleSlider_valueChanged(int value) {
    on_hSpinSlider_valueChanged(value);
}

void MainWindow::on_themeComboBox_currentIndexChanged(int index) {
    surfaceChart->activeTheme()->setType(Q3DTheme::Theme(index));

}

void MainWindow::on_barComboBox_currentIndexChanged(int index) {
    QSurface3DSeries* series = surfaceChart->seriesList().at(0);
    series->setMesh(QAbstract3DSeries::Mesh(index));
}

void MainWindow::on_selectComboBox_currentIndexChanged(int index) {
    if (index == 9) {
        surfaceChart->setSelectionMode(QAbstract3DGraph::SelectionFlag(16));
    } else {
        surfaceChart->setSelectionMode(QAbstract3DGraph::SelectionFlag(index));
    }
}

void MainWindow::on_fontSpinBox_valueChanged(int value) {
    QFont font = surfaceChart->activeTheme()->font();
    font.setPointSize(value);
    surfaceChart->activeTheme()->setFont(font);
}

void MainWindow:: on_itemBtn_clicked() {
    QSurface3DSeries* series = surfaceChart->seriesList().at(0);
    QColor color = series->baseColor();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        series->setBaseColor(color);
        //启动渐变
        series->setColorStyle(Q3DTheme::ColorStyleUniform);
    }
}

void MainWindow::on_itemGradBtn1_clicked() {
    //按钮渐变色
    QLinearGradient gradColor1(0,0,100,0);
    gradColor1.setColorAt(1.0, Qt::black);
    gradColor1.setColorAt(0.67, Qt::blue);
    gradColor1.setColorAt(0.33, Qt::red);
    gradColor1.setColorAt(0.0, Qt::yellow);
    QSurface3DSeries* series = surfaceChart->seriesList().at(0);
    series->setBaseGradient(gradColor1);
    //启动渐变
    series->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void MainWindow::on_itemGradBtn2_clicked() {
    //按钮渐变色
    QLinearGradient gradColor1(0,0,100,0);
    gradColor1.setColorAt(1.0, Qt::darkGreen);
    gradColor1.setColorAt(0.5, Qt::yellow);
    gradColor1.setColorAt(0.2, Qt::red);
    gradColor1.setColorAt(0.0, Qt::darkRed);
    QSurface3DSeries* series = surfaceChart->seriesList().at(0);
    series->setBaseGradient(gradColor1);
    //启动渐变
    series->setColorStyle(Q3DTheme::ColorStyleObjectGradient);
}

void MainWindow::on_backgroundCheckBox_clicked(bool checked) {
    surfaceChart->activeTheme()->setBackgroundEnabled(checked);
}

void MainWindow::on_gridCheckBox_clicked(bool checked) {
    surfaceChart->activeTheme()->setGridEnabled(checked);
}

void MainWindow::on_smoothCheckBox_clicked(bool checked) {
    QSurface3DSeries* series = surfaceChart->seriesList().at(0);
    series->setMeshSmooth(checked);
}

void MainWindow::on_invertShadeCheckBox_clicked(bool checked) {
    surfaceChart->setReflection(checked);
}

void MainWindow::on_invertValueCheckBox_clicked(bool checked) {
    QValue3DAxis* axisValue = surfaceChart->axisY();
    axisValue->setReversed(checked);
}

void MainWindow::on_itemLabelCheckBox_clicked(bool checked) {
    // surfaceChart->activeTheme()->setLabelBorderEnabled(checked);
    QSurface3DSeries* series = surfaceChart->seriesList().at(0);
    series->setItemLabelVisible(checked);
}

void MainWindow::axisTitleCheckBox_clicked(bool checked) {
    surfaceChart->axisY()->setTitleVisible(checked);
    surfaceChart->axisX()->setTitleVisible(checked);
    surfaceChart->axisZ()->setTitleVisible(checked);
}

void MainWindow::on_axisLabelCheckBox_clicked(bool checked) {
    surfaceChart->activeTheme()->setLabelBackgroundEnabled(checked);
}


void MainWindow::on_surfaceComboBox_currentIndexChanged(int index) {
    QSurface3DSeries* series = surfaceChart->seriesList().at(0);
    series->setDrawMode(QSurface3DSeries::DrawFlag(index+1));
}
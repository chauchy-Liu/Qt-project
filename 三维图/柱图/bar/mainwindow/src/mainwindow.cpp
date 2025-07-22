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
    splitter->addWidget(createWindowContainer(barChart));
    //把splitter添加进父窗口
    setCentralWidget(splitter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initGraph3D() {
    //分配空间
    barChart = new Q3DBars;
    //创建坐标
    QValue3DAxis* axisValue = new QValue3DAxis;
    axisValue->setTitle("Value");
    axisValue->setTitleVisible(true);

    QCategory3DAxis* axisCol = new QCategory3DAxis;
    axisCol->setTitle("Column");
    axisCol->setTitleVisible(true);

    QCategory3DAxis* axisRow = new QCategory3DAxis;
    axisRow->setTitle("Row");
    axisRow->setTitleVisible(true);
    //图表添加坐标轴
    barChart->setValueAxis(axisValue);
    barChart->setColumnAxis(axisCol);
    barChart->setRowAxis(axisRow);

    //坐标标签
    QStringList rowLabs;
    QStringList colLabs;
    rowLabs << "row1" << "row2" << "row3" << "row4";
    colLabs << "col1" << "col2" << "col3" << "col4" << "col5";
    axisRow->setLabels(rowLabs);
    axisCol->setLabels(colLabs);



    //创建序列
    QBar3DSeries* series = new QBar3DSeries;
    //网格块
    series->setMesh(QAbstract3DSeries::MeshCylinder);
    series->setName("bar");
    series->setItemLabelFormat("(@rowLabel,@colLabel):%.1f");//@valueLabel

    //图表添加序列
    barChart->addSeries(series);

    //添加数据
    QBarDataArray* dataArray = new QBarDataArray;
    //设置数组长度,即需要记录多少行
    dataArray->reserve(rowLabs.count());
    //创建一行数据项
    QBarDataRow* dataRow0 = new QBarDataRow;
    *dataRow0 << 1 << 2 << 3 << 4 << 5;
    QBarDataRow* dataRow1 = new QBarDataRow;
    *dataRow1 << 5 << 5 << 5 << 5 << 5;
    QBarDataRow* dataRow2 = new QBarDataRow;
    *dataRow2 << 5 << 4 << 3 << 4 << 5;
    QBarDataRow* dataRow3 = new QBarDataRow;
    *dataRow3 << 5 << 4 << 3 << 4 << 5;
    //将行数据添加进行数组
    *dataArray << dataRow0 << dataRow1 << dataRow2 << dataRow3;

    //用代理添加数据
    series->dataProxy()->resetArray(dataArray);

    //初始化图表其他属性
    // int xRot = ui->hSpinSlider->value();
    // int yRot = ui->vSpinSlider->value();
    // int zoom = ui->scaleSlider->value();
    // barChart->scene()->activeCamera()->setCameraPosition(xRot,yRot,zoom);
    barChart->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPreset(1));
    ui->sightComboBox->setCurrentIndex(2);
    ui->hSpinSlider->setValue(barChart->scene()->activeCamera()->xRotation());
    ui->vSpinSlider->setValue(barChart->scene()->activeCamera()->yRotation());
    ui->scaleSlider->setValue(barChart->scene()->activeCamera()->zoomLevel());
    ui->barComboBox->setCurrentText("Cylinder");
    // ui->barComboBox->setCurrentIndex(1);
    ui->selectComboBox->setCurrentIndex(1);
    ui->backgroundCheckBox->setChecked(true);
    ui->gridCheckBox->setChecked(true);
    ui->itemLabelCheckBox->setChecked(true);
    ui->axisLabelCheckBox->setChecked(true);
    QFont font = barChart->activeTheme()->font();
    ui->fontSpinBox->setValue(font.pointSize());
}

void MainWindow::on_sightComboBox_activated(int index) {
    //坐标系视角
    barChart->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPreset(index-1));
}

void MainWindow::on_hSpinSlider_valueChanged(int value) {
    Q_UNUSED(value);
    int xRot = ui->hSpinSlider->value();
    int yRot = ui->vSpinSlider->value();
    int zoom = ui->scaleSlider->value();
    //计算机图形学
    barChart->scene()->activeCamera()->setCameraPosition(xRot, yRot, zoom);
}

void MainWindow::on_vSpinSlider_valueChanged(int value) {
    on_hSpinSlider_valueChanged(value);
}

void MainWindow::on_scaleSlider_valueChanged(int value) {
    on_hSpinSlider_valueChanged(value);
}

void MainWindow::on_themeComboBox_currentIndexChanged(int index) {
    barChart->activeTheme()->setType(Q3DTheme::Theme(index));

}

void MainWindow::on_barComboBox_currentIndexChanged(int index) {
    QBar3DSeries* series = barChart->seriesList().at(0);
    series->setMesh(QAbstract3DSeries::Mesh(index));
}

void MainWindow::on_selectComboBox_currentIndexChanged(int index) {
    if (index == 9) {
        barChart->setSelectionMode(QAbstract3DGraph::SelectionFlag(16));
    } else {
        barChart->setSelectionMode(QAbstract3DGraph::SelectionFlag(index));
    }
}

void MainWindow::on_fontSpinBox_valueChanged(int value) {
    QFont font = barChart->activeTheme()->font();
    font.setPointSize(value);
    barChart->activeTheme()->setFont(font);
}

void MainWindow:: on_itemBtn_clicked() {
    QBar3DSeries* series = barChart->seriesList().at(0);
    QColor color = series->baseColor();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        series->setBaseColor(color);
    }
}

void MainWindow::on_backgroundCheckBox_clicked(bool checked) {
    barChart->activeTheme()->setBackgroundEnabled(checked);
}

void MainWindow::on_gridCheckBox_clicked(bool checked) {
    barChart->activeTheme()->setGridEnabled(checked);
}

void MainWindow::on_smoothCheckBox_clicked(bool checked) {
    QBar3DSeries* series = barChart->seriesList().at(0);
    series->setMeshSmooth(checked);
}

void MainWindow::on_invertShadeCheckBox_clicked(bool checked) {
    barChart->setReflection(checked);
}

void MainWindow::on_invertValueCheckBox_clicked(bool checked) {
    QValue3DAxis* axisValue = barChart->valueAxis();
    axisValue->setReversed(checked);
}

void MainWindow::on_itemLabelCheckBox_clicked(bool checked) {
    // barChart->activeTheme()->setLabelBorderEnabled(checked);
    QBar3DSeries* series = barChart->seriesList().at(0);
    series->setItemLabelVisible(checked);
}

void MainWindow::axisTitleCheckBox_clicked(bool checked) {
    barChart->valueAxis()->setTitleVisible(checked);
    barChart->rowAxis()->setTitleVisible(checked);
    barChart->columnAxis()->setTitleVisible(checked);
}

void MainWindow::on_axisLabelCheckBox_clicked(bool checked) {
    barChart->activeTheme()->setLabelBackgroundEnabled(checked);
}
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Qtime>
#include <QCursor>

#include "pendialog.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    //设置splitter
    int splitterSize = ui->splitter->size().width();
    qDebug() << "splitterSize: " << splitterSize;
    int leftFactor = 10; //splitterSize/25;
    int rightFactor = splitterSize - leftFactor; //splitterSize/25*24;
    qDebug() << "leftSize: " << leftFactor << " rightSize: " << rightFactor;
    QList<int> sizeFactor = {leftFactor, rightFactor};
    ui->splitter->setSizes(sizeFactor);
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 9);

    //创建QChart绘图
    createChart();
    //准备数据
    prepareData();
    //更新工具窗口
    updateBoxFromChart();
    //给QChartView安装事件过滤器
    // ui->chartView->installEventFilter(this);
    //参数面板初始值
    // cursorDataVisible.insert("x", false);
    // cursorDataVisible.insert("y", false);
    // ui->checkBox11->setChecked(false);
    ui->minDSpinBox->setMinimum(-999999.99);  // 允许负数
    ui->maxDSpinBox->setMaximum(999999.99);
    on_xRdBtn_clicked(true);
    xyCoord = new QLabel("屏幕坐标(screenPos)X:,Y: ;应用窗体坐标(appPos)X:,Y: ;视图坐标(viewPos)X:,Y: ;场景坐标(scenePos)X:,Y: ;图表坐标(chartPos)X:,Y:");
    ui->statusbar->addWidget(xyCoord);
    //信号与槽
    connect(ui->quitAction, &QAction::triggered, this, &MainWindow::on_quitActionTriggered);
    connect(ui->drawAction, &QAction::triggered, this, &MainWindow::on_drawActionTriggered);
    connect(ui->zoomInAction, &QAction::triggered, this, &MainWindow::on_zoominActionTriggered);
    connect(ui->zoomOutAction, &QAction::triggered, this, &MainWindow::on_zoomoutActionTriggered);
    connect(ui->zoomOriginAction, &QAction::triggered, this, &MainWindow::on_zoomActionTriggered);
    connect(ui->chartView, &CustomChartView::signalMouseEvent, this, &MainWindow::theSlotMouseEvent);
    // connect(ui->chartView, &CustomChartView::signalKeyEvent, this, &MainWindow::theSlotKeyEvent);
    connect(ui->chartView, &CustomChartView::signalWheelEvent, this, &MainWindow::theSlotWheelEvent);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::createChart() {
    //图表
    QChart *chart = new QChart;
    chart->setTitle("简单曲线");
    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
    //折线序列
    QLineSeries *series0 = new QLineSeries;
    QLineSeries *series1 = new QLineSeries;
    chart->addSeries(series0);
    chart->addSeries(series1);
    series0->setName("Sin曲线");
    series1->setName("Cos曲线");
    cursorDataVisible[series0->name()] = false;
    cursorDataVisible[series1->name()] = false;
    dataAutoLine[series0->name()] = false;
    dataAutoLine[series1->name()] = false;
    curSeries = series0;
    //画笔
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    pen.setColor(Qt::red);
    series0->setPen(pen);
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::blue);
    series1->setPen(pen);
    //坐标轴
    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;
    // chart->createDefaultAxes(); //创建默认轴
    // chart->setAxisX(axisX, series0);
    // chart->setAxisY(axisY, series0);
    // chart->setAxisX(axisX, series1);
    // chart->setAxisY(axisY, series1);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series0->attachAxis(axisX);
    series0->attachAxis(axisY);
    series1->attachAxis(axisX);
    series1->attachAxis(axisY);
    curAxis = axisX;

    // axisX->setRange(0, 10);
    axisX->applyNiceNumbers(); //自动调整X轴范围以适应数据范围
    axisX->setLabelFormat("%.1f");
    axisX->setTickCount(11);
    axisX->setMinorTickCount(4);
    axisX->setTitleText("time(secs)");
    // axisY->setRange(-1.5, 1.5);
    axisY->applyNiceNumbers(); //自动调整Y轴范围以适应数据范围
    axisY->setLabelFormat("%.1f");
    axisY->setTickCount(3);
    axisY->setMinorTickCount(4);
    axisY->setTitleText("value");

    //legend信号与槽
    foreach (auto marker, ui->chartView->chart()->legend()->markers()) {
        connect(marker, &QLegendMarker::clicked, this, &MainWindow::onLegendMarkClicked);
    }
}

/**
 * @brief 给折线序列提供数据
 */
void MainWindow::prepareData() {
    QLineSeries *series0 = dynamic_cast<QLineSeries *>(ui->chartView->chart()->series().at(0));
    QLineSeries *series1 = dynamic_cast<QLineSeries *>(ui->chartView->chart()->series().at(1));
    int cnt = 100;
    double interval = 0.1;
    series0->clear();
    series1->clear();
    srand(QTime::currentTime().second());
    qreal t = 0, y1, y2, rd, maxValueX, minValueX, maxValueY, minValueY;
    for (int i = 0; i < cnt; i++) {
        rd = (rand() % 10) - 5;
        y1 = qSin(t) + rd / 50;
        y2 = qCos(t) + rd / 50;
        if (i == 0) {
            maxValueX = t;
            minValueX = t;
            maxValueY = y1 > y2 ? y1 : y2;
            minValueY = y1 < y2 ? y1 : y2;
        } else {
            if (t > maxValueX) maxValueX = t;
            if (t < minValueX) minValueX = t;
            if (y1 > maxValueY) maxValueY = y1;
            if (y2 > maxValueY) maxValueY = y2;
            if (y1 < minValueY) minValueY = y1;
            if (y2 < minValueY) minValueY = y2;
        }
        series0->append(t, y1);
        series1->append(t, y2);
        t = t + interval;
    }
    //调整坐标轴范围
    QValueAxis *axisX = dynamic_cast<QValueAxis *>(ui->chartView->chart()->axisX());
    QValueAxis *axisY = dynamic_cast<QValueAxis *>(ui->chartView->chart()->axisY());
    qreal marginDelta = 1;
    axisX->setRange(minValueX - marginDelta, maxValueX + marginDelta);
    axisY->setRange(minValueY - marginDelta, maxValueY + marginDelta);
}

//#########################################################
//工具栏
//#########################################################

/**
 * @brief 根据图表更新参数窗口
 */
void MainWindow::updateBoxFromChart() {
    QChart *chart = ui->chartView->chart();
    ui->editTitle->setText(chart->title());

    QMargins margins = chart->margins();
    ui->sideSpinBox1->setValue(margins.left());
    ui->sideSpinBox2->setValue(margins.right());
    ui->sideSpinBox3->setValue(margins.top());
    ui->sideSpinBox4->setValue(margins.bottom());

    Qt::Alignment alignFlag = chart->legend()->alignment();
    if (chart->legend() != nullptr) {
        ui->checkBox1->setChecked(true);
        switch (alignFlag) {
            case Qt::AlignLeft: {
                ui->rdBtn3->setChecked(true);
                break;
            }
            case Qt::AlignRight: {
                ui->rdBtn4->setChecked(true);
                break;
            }
            case Qt::AlignTop: {
                ui->rdBtn1->setChecked(true);
                break;
            }
            case Qt::AlignBottom: {
                ui->rdBtn2->setChecked(true);
                break;
            }
        }
        if (chart->legend()->isBackgroundVisible()) {
            ui->checkBox2->setChecked(true);
        } else {
            ui->checkBox2->setChecked(false);
        }
    } else {
        ui->checkBox1->setChecked(false);
        ui->rdBtn1->setChecked(false);
        ui->rdBtn1->setEnabled(false);
        ui->rdBtn2->setChecked(false);
        ui->rdBtn2->setEnabled(false);
        ui->rdBtn3->setChecked(false);
        ui->rdBtn3->setEnabled(false);
        ui->rdBtn4->setChecked(false);
        ui->rdBtn4->setEnabled(false);
        ui->checkBox2->setChecked(false);
        ui->checkBox2->setEnabled(false);
    }
}


void MainWindow::on_quitActionTriggered() {
    this->close();
}

void MainWindow::on_drawActionTriggered() {
    prepareData();
}

void MainWindow::on_zoominActionTriggered() {
    ui->chartView->chart()->zoom(1.2);
}

void MainWindow::on_zoomoutActionTriggered() {
    ui->chartView->chart()->zoom(0.8);
}

void MainWindow::on_zoomActionTriggered() {
    ui->chartView->chart()->zoomReset();
}

void MainWindow::onLegendMarkClicked() {
    QLegendMarker* marker = (QLegendMarker*)sender();
    marker->series()->setVisible(!marker->series()->isVisible());
    marker->setVisible(true);

    qreal alpha;
    if (marker->series()->isVisible()) {
        marker->setLabelBrush(QBrush(Qt::black));
        // marker->setBrush(QBrush(Qt::black));
    } else {
        marker->setLabelBrush(QBrush(Qt::gray));
        // marker->setBrush(QBrush(Qt::gray));
    }
}

//#########################################################
//图表
//#########################################################
void MainWindow::on_btnSetTitle_clicked() {
    QString title = ui->editTitle->text();
    ui->chartView->chart()->setTitle(title);
}

void MainWindow::on_btnSetTitleFont_clicked() {
    //当前Font
    QFont font = ui->chartView->chart()->titleFont();
    bool ok = false;
    font = QFontDialog::getFont(&ok, font);
    if (ok) {
        ui->chartView->chart()->setTitleFont(font);
    }
}

void MainWindow::on_rdBtn1_clicked() {
    QLegend *legend = ui->chartView->chart()->legend();
    legend->setAlignment(Qt::AlignTop);
}

void MainWindow::on_rdBtn2_clicked() {
    QLegend *legend = ui->chartView->chart()->legend();
    legend->setAlignment(Qt::AlignBottom);
}

void MainWindow::on_rdBtn3_clicked() {
    QLegend *legend = ui->chartView->chart()->legend();
    legend->setAlignment(Qt::AlignLeft);
}

void MainWindow::on_rdBtn4_clicked() {
    QLegend *legend = ui->chartView->chart()->legend();
    legend->setAlignment(Qt::AlignRight);
}

void MainWindow::on_checkBox2_clicked() {
    QLegend *legend = ui->chartView->chart()->legend();
    if (ui->checkBox2->isChecked()) {
        legend->setBackgroundVisible(true);
    } else {
        legend->setBackgroundVisible(false);
    }
}

void MainWindow::on_checkBox1_clicked() {
    QLegend *legend = ui->chartView->chart()->legend();
    if (ui->checkBox1->checkState() == Qt::Checked) {
        ui->rdBtn1->setEnabled(true);
        ui->rdBtn2->setEnabled(true);
        ui->rdBtn3->setEnabled(true);
        ui->rdBtn4->setEnabled(true);
        ui->checkBox2->setEnabled(true);
        legend->setVisible(true);
    } else {
        ui->rdBtn1->setChecked(false);
        ui->rdBtn1->setEnabled(false);
        ui->rdBtn2->setChecked(false);
        ui->rdBtn2->setEnabled(false);
        ui->rdBtn3->setChecked(false);
        ui->rdBtn3->setEnabled(false);
        ui->rdBtn4->setChecked(false);
        ui->rdBtn4->setEnabled(false);
        ui->checkBox2->setChecked(false);
        ui->checkBox2->setEnabled(false);
        legend->setVisible(false);
    }
}

void MainWindow::on_btnFont_clicked() {
    QFont font = ui->chartView->chart()->legend()->font();
    bool ok = false;
    font = QFontDialog::getFont(&ok, font);
    if (ok) {
        ui->chartView->chart()->legend()->setFont(font);
    }
}

void MainWindow::on_btnLabelColor_clicked() {
    QColor color = ui->chartView->chart()->legend()->color();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        ui->chartView->chart()->legend()->setColor(color);
    }
}

void MainWindow::on_sideBtn_clicked() {
    QMargins margins;
    margins.setLeft(ui->sideSpinBox1->value());
    margins.setRight(ui->sideSpinBox2->value());
    margins.setTop(ui->sideSpinBox3->value());
    margins.setBottom(ui->sideSpinBox4->value());
    ui->chartView->chart()->setMargins(margins);
}

void MainWindow::on_fontColorBtn2_clicked() {
    QColor color = ui->chartView->chart()->legend()->labelColor();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        ui->chartView->chart()->legend()->setLabelColor(color);
    }
}

void MainWindow::on_labelColorBtn2_clicked() {
    QColor color = ui->chartView->chart()->legend()->labelBrush().color();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        ui->chartView->chart()->legend()->setLabelBrush(color);
    }
}

void MainWindow::on_comboBox1_currentIndexChanged(int index) {
    ui->chartView->chart()->setAnimationOptions((QChart::AnimationOptions) index);
}

void MainWindow::on_comboBox2_currentIndexChanged(int index) {
    ui->chartView->chart()->setTheme(QChart::ChartTheme(index));
}
//#########################################################
//折线序列
//#########################################################
void MainWindow::on_curveTypeRdBtn1_clicked() {
    curSeries = dynamic_cast<QLineSeries *>(ui->chartView->chart()->series().at(0));
    //更新参数窗口
    ui->curveNameLineEdit->setText(curSeries->name());
    ui->checkBox3->setChecked(curSeries->isVisible());
    ui->checkBox4->setChecked(curSeries->pointsVisible());
    ui->transparencyHSlider->setValue((int)(curSeries->opacity()*100));
    ui->checkBox5->setChecked(curSeries->pointLabelsVisible());
    ui->checkBox11->setChecked(cursorDataVisible[curSeries->name()]);
    ui->checkBox12->setChecked(dataAutoLine[curSeries->name()]);
}

void MainWindow::on_curveTypeRdBtn2_clicked() {
    curSeries = dynamic_cast<QLineSeries *>(ui->chartView->chart()->series().at(1));
    //更新参数窗口
    ui->curveNameLineEdit->setText(curSeries->name());
    ui->checkBox3->setChecked(curSeries->isVisible());
    ui->checkBox4->setChecked(curSeries->pointsVisible());
    ui->transparencyHSlider->setValue((int)(curSeries->opacity()*100));
    ui->checkBox5->setChecked(curSeries->pointLabelsVisible());
    ui->checkBox11->setChecked(cursorDataVisible[curSeries->name()]);
    ui->checkBox12->setChecked(dataAutoLine[curSeries->name()]);
}

void MainWindow::on_btnCurveName_clicked() {
    bool oldValue = cursorDataVisible[curSeries->name()];
    bool oldValueLine = dataAutoLine[curSeries->name()];
    cursorDataVisible.remove(curSeries->name());
    dataAutoLine.remove(curSeries->name());
    curSeries->setName(ui->curveNameLineEdit->text());
    cursorDataVisible[curSeries->name()] = oldValue;
    dataAutoLine[curSeries->name()] = oldValueLine;
}

void MainWindow::on_checkBox3_clicked(bool checked) {
    curSeries->setVisible(checked);
}

void MainWindow::on_checkBox4_clicked(bool checked) {
    curSeries->setPointsVisible(checked);
}

void MainWindow::on_curveColorBtn_clicked() {
    QColor color = curSeries->color();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        curSeries->setColor(color);
    }
}

void MainWindow::on_penBtn_clicked() {
    bool ok = false;
    QPen pen = curSeries->pen();
    pen = PenDialog::getPen(pen, ok);
    if (ok) {
        curSeries->setPen(pen);
    }
}

void MainWindow::on_transparencyHSlider_valueChanged(int value) {
    curSeries->setOpacity(value/100.);
}

void MainWindow::on_checkBox5_clicked(bool checked) {
    curSeries->setPointLabelsVisible(checked);
}

void MainWindow::on_labelColorBtn_clicked() {
    QColor color = curSeries->pointLabelsColor();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        curSeries->setPointLabelsColor(color);
    }
}

void MainWindow::on_labelFontBtn_clicked() {
    QFont font = curSeries->pointLabelsFont();
    bool ok = false;
    font = QFontDialog::getFont(&ok, font);
    if (ok) {
        curSeries->setPointLabelsFont(font);
    }
}

void MainWindow::on_labelFormatRdBtn1_clicked() {
    curSeries->setPointLabelsFormat("@yPoint");
    // ui->chartView->formatFlag = true;
}

void MainWindow::on_labelFormatRdBtn2_clicked() {
    curSeries->setPointLabelsFormat("(@xPoint,@yPoint)");
    // ui->chartView->formatFlag = false;
}

void MainWindow::on_checkBox11_clicked(bool checked) {
    // ui->chartView->isVisible = checked;
    // ui->chartView->setCurSeries(curSeries);
    if (checked == true) {
        connect(curSeries, &QLineSeries::hovered, this, &MainWindow::on_hoverSeries_cursor);
        // curSeries
        cursorDataVisible[curSeries->name()] = true;
    } else {
        disconnect(curSeries, &QLineSeries::hovered, this, &MainWindow::on_hoverSeries_cursor);
        cursorDataVisible[curSeries->name()] = false;
    }
}

void MainWindow::on_checkBox12_clicked(bool checked) {
    if (checked == true) {
        connect(curSeries, &QLineSeries::hovered, this, &MainWindow::updateXYGuideLine);
        // curSeries
        dataAutoLine[curSeries->name()] = true;
        // paintTimer = new QTimer(this);
        // paintTimer->start(10);
        // connect(paintTimer, &QTimer::timeout, [=]() {
        //     ui->chartView->update();
        // });
        xGuideSeries = new QLineSeries; //QSplineSeries
        yGuideSeries = new QLineSeries; //QSplineSeries
        ui->chartView->chart()->addSeries(xGuideSeries);
        ui->chartView->chart()->addSeries(yGuideSeries);
        QValueAxis* axisX = dynamic_cast<QValueAxis *>(ui->chartView->chart()->axes(Qt::Horizontal)[0]);
        QValueAxis* axisY = dynamic_cast<QValueAxis *>(ui->chartView->chart()->axes(Qt::Vertical)[0]);
        xGuideSeries->attachAxis(axisX);
        xGuideSeries->attachAxis(axisY);
        yGuideSeries->attachAxis(axisX);
        yGuideSeries->attachAxis(axisY);
        QPen pen;
        pen.setWidth(2);
        pen.setStyle(Qt::DashDotLine);
        xGuideSeries->setPen(pen);
        yGuideSeries->setPen(pen);
        xGuideSeries->setColor(QColor(74,101,87, 100));
        yGuideSeries->setColor(QColor(74,101,87, 100));
        ui->chartView->chart()->legend()->markers()[2]->setVisible(false);
        ui->chartView->chart()->legend()->markers()[3]->setVisible(false);
    } else {
        disconnect(curSeries, &QLineSeries::hovered, this, &MainWindow::updateXYGuideLine);
        dataAutoLine[curSeries->name()] = false;
        // disconnect(paintTimer, &QTimer::timeout, this, &MainWindow::updateXYGuideLine);
        // paintTimer->stop();
        // delete paintTimer;
        ui->chartView->chart()->removeSeries(xGuideSeries);
        ui->chartView->chart()->removeSeries(yGuideSeries);
        delete xGuideSeries;
        delete yGuideSeries;
    }
}

void MainWindow::on_hoverSeries_cursor(QPointF curPoint, bool state) {
    // QPoint tempPoint;
    // tempPoint.setX(qRound(point.x()));
    // tempPoint.setY(qRound(point.y()));
    if (state) { // && curSeries->points().contains(tempPoint)
        // QPointF closestPoint;
        // qreal minDist = std::numeric_limits<qreal>::max();
        // for (const QPointF &point : curSeries->points()) {
        //     qreal dist = qSqrt(qPow(point.x() - curPoint.x(), 2) + qPow(point.y() - curPoint.y(), 2));
        //     if (dist < minDist) {
        //         minDist = dist;
        //         closestPoint = point;
        //     }
        // }
        qDebug() << "QLineSeries坐标:" << curPoint;//closestPoint
        qDebug() << "QCursor坐标:" << QCursor::pos();
        if (ui->labelFormatRdBtn1->isChecked()) {
            QToolTip::showText(QCursor::pos(), QString("Y: %2").arg(curPoint.y()), this);
        } else {
            QToolTip::showText(QCursor::pos(), QString("X: %1\nY: %2").arg(curPoint.x()).arg(curPoint.y()), this);
        }
    }
}
//#########################################################
//坐标轴
//#########################################################
void MainWindow::on_xRdBtn_clicked(bool checked) {
    //获取当前轴
    QList<QAbstractAxis*> axes;
    axes = ui->chartView->chart()->axes(Qt::Horizontal);
    curAxis = dynamic_cast<QValueAxis*>(axes[0]);
    //根据图表设置参数面板
    ui->minDSpinBox->setValue(curAxis->min());
    ui->maxDSpinBox->setValue(curAxis->max());
    ui->checkBox6->setChecked(curAxis->isVisible());
    ui->titleLineEdit->setText(curAxis->titleText());
    ui->checkBox7->setChecked(curAxis->isTitleVisible());
    ui->labelFormatLineEdit->setText(curAxis->labelFormat());
    ui->checkBox8->setChecked(curAxis->isLineVisible());
    ui->tickSpinBox->setValue(curAxis->tickCount());
    ui->checkBox9->setChecked(curAxis->isGridLineVisible());
    ui->minorTickSpinBox->setValue(curAxis->minorTickCount());
    ui->checkBox10->setChecked(curAxis->isMinorGridLineVisible());
}

void MainWindow::on_yRdBtn_clicked(bool checked) {
    //获取当前轴
    QList<QAbstractAxis*> axes;
    axes = ui->chartView->chart()->axes(Qt::Vertical);
    curAxis = dynamic_cast<QValueAxis*>(axes[0]);
    //根据图表设置参数面板
    ui->minDSpinBox->setValue(curAxis->min());
    ui->maxDSpinBox->setValue(curAxis->max());
    ui->checkBox6->setChecked(curAxis->isVisible());
    ui->titleLineEdit->setText(curAxis->titleText());
    ui->checkBox7->setChecked(curAxis->isTitleVisible());
    ui->labelFormatLineEdit->setText(curAxis->labelFormat());
    ui->checkBox8->setChecked(curAxis->isGridLineVisible());
    ui->tickSpinBox->setValue(curAxis->tickCount());
    ui->checkBox9->setChecked(curAxis->isLineVisible());
    ui->minorTickSpinBox->setValue(curAxis->minorTickCount());
    ui->checkBox10->setChecked(curAxis->isMinorGridLineVisible());
}

void MainWindow::on_rangeBtn_clicked() {
    curAxis->setRange(ui->minDSpinBox->value(), ui->maxDSpinBox->value());
}

void MainWindow::on_checkBox6_clicked(bool checked) {
    curAxis->setVisible(checked);
}

void MainWindow::on_titleBtn_clicked() {
    curAxis->setTitleText(ui->titleLineEdit->text());
}

void MainWindow::on_fontBtn1_clicked() {
    QFont font = curAxis->titleFont();
    bool ok;
    font = QFontDialog::getFont(&ok, font);
    if (ok) {
        curAxis->setTitleFont(font);
    }
}

void MainWindow::on_checkBox7_clicked(bool checked) {
    curAxis->setTitleVisible(checked);
}

void MainWindow::on_labelFormatBtn_clicked() {
    curAxis->setLabelFormat(ui->labelFormatLineEdit->text());
}

void MainWindow::on_fontColorBtn_clicked() {
    QColor color = curAxis->labelsColor();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        curAxis->setLabelsColor(color);
    }
}

void MainWindow::on_fontBtn2_clicked() {
    QFont font = curAxis->labelsFont();
    bool ok;
    font = QFontDialog::getFont(&ok, font);
    if (ok) {
        curAxis->setLabelsFont(font);
    }
}
void MainWindow::on_checkBox8_clicked(bool checked) {
    curAxis->setGridLineVisible(checked);
}
void MainWindow::on_gridColorBtn_clicked() {
    QColor color = curAxis->gridLineColor();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        curAxis->setGridLineColor(color);
    }
}
void MainWindow::on_gridPenBtn_clicked() {
    QPen pen = curAxis->gridLinePen();
    bool ok;
    pen = PenDialog::getPen(pen, ok);
    if (ok) {
        curAxis->setGridLinePen(pen);
    }
}
void MainWindow::on_tickSpinBox_valueChanged(int value) {
    curAxis->setTickCount(value);
}
void MainWindow::on_checkBox9_clicked(bool checked) {
    curAxis->setLineVisible(checked);
}
void MainWindow::on_tickColorBtn_clicked() {
    QColor color = curAxis->linePenColor();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        curAxis->setLinePenColor(color);
    }
}
void MainWindow::on_tickPenBtn_clicked() {
    QPen pen = curAxis->linePen();
    bool ok;
    pen = PenDialog::getPen(pen, ok);
    if (ok) {
        curAxis->setLinePen(pen);
    }
}
void MainWindow::on_minorTickSpinBox_valueChanged(int value) {
    curAxis->setMinorTickCount(value);
}
void MainWindow::on_checkBox10_clicked(bool checked) {
    curAxis->setMinorGridLineVisible(checked);
}
void MainWindow::on_minorTickPenBtn_clicked() {
    QPen pen = curAxis->minorGridLinePen();
    bool ok;
    pen = PenDialog::getPen(pen, ok);
    if (ok) {
        curAxis->setMinorGridLinePen(pen);
    }
}
void MainWindow::on_minorTickColorBtn_clicked() {
    QColor color = curAxis->minorGridLineColor();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        curAxis->setMinorGridLineColor(color);
    }
}



void MainWindow::theSlotMouseEvent(int eventId, QMouseEvent *event) {
    if (eventId == 0){  //单击按下
        isMousePressed = true;
        // QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        pressedPoint = event->pos();
        ui->chartView->setCursor(Qt::ClosedHandCursor);
    } else if (eventId == 1) {  //鼠标移动
        // qDebug() << "鼠标移动位置：" << event->pos();
        QString xyStr = QString("屏幕坐标(screenPos)X:%1,Y:%2 ;应用窗体坐标(appPos)X:%3,Y:%4 ;视图坐标(viewPos)X:%5,Y:%6 ;场景坐标(scenePos)X:%7,Y:%8 ;图表坐标(chartPos)X:%9,Y:%10");
        QPointF posView = event->pos();
        QPointF posApp = event->scenePosition();
        QPointF posScreen = event->globalPosition();//event->globalPosition()和QCursor::pos()坐标一样是屏幕坐标
        QPointF posScene = ui->chartView->mapToScene(posView.toPoint());// ui->chartView->chart()->mapToScene(ui->chartView->chart()->mapToValue(posView));
        QPointF posChart = ui->chartView->chart()->mapToValue(posView);
        xyStr = xyStr.arg(posScreen.toPoint().x()).arg(posScreen.toPoint().y()).arg(posApp.toPoint().x()).arg(posApp.toPoint().y()).arg(posView.toPoint().x()).arg(posView.toPoint().y()).arg(posScene.toPoint().x()).arg(posScene.toPoint().y()).arg(posChart.x(), 0, 'f', 1).arg(posChart.y(), 0, 'f', 1);
        xyCoord->setText(xyStr);
        if (isMousePressed && !isKeyPressed) {
            ui->chartView->setCursor(Qt::ClosedHandCursor);
            // QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            ui->chartView->chart()->scroll(-(event->pos().x() - pressedPoint.x()) / 100., //1、曲线移动方向和图表坐标系的移动方向相反，2、图表坐标系的y轴向上而chartview组件的坐标系y轴向下
                               (event->pos().y() - pressedPoint.y()) / 100.);
            // updateXYGuideLine();
            //更新参数面板坐标轴范围
            ui->minDSpinBox->setValue(curAxis->min());
            ui->maxDSpinBox->setValue(curAxis->max());
            //更新绘图(为了画辅助线不缺失)
            // ui->chartView->update();
            // ui->chartView->cursor().setShape(Qt::ClosedHandCursor);
            // ui->chartView->setCursor(Qt::ClosedHandCursor);
        }
    } else if (eventId == 2) {  //单击抬起
        isMousePressed = false;
        ui->chartView->setCursor(Qt::ArrowCursor);
    } else if (eventId == 3) {  //双击
        ui->chartView->chart()->zoomReset();
        // updateXYGuideLine();
    }

}

// void MainWindow::theSlotKeyEvent(int eventId, QKeyEvent *event) {
//     if (eventId == 0) {
//         isKeyPressed = true;
//     }else if (eventId == 1) {
//         isKeyPressed = false;
//     }
// }

void MainWindow::theSlotWheelEvent(QWheelEvent *event) {
    int delta = event->angleDelta().y();
    if (isKeyPressed) {
        //当前鼠标位置
        // event->pos();
        //视图坐标
        QPoint curPointView =event->position().toPoint();//QCursor::pos();
        qDebug() << "###################################################";
        qDebug() << "视图坐标鼠标放大位置：" << curPointView;//event->pos();
        QPointF curPointScene = ui->chartView->mapToScene(curPointView);
        qDebug() << "场景坐标鼠标放大位置：" << curPointScene;//event->pos();
        QPointF curPointChart = ui->chartView->chart()->mapToValue(curPointScene, curSeries); //QPoint curPointChart
        qDebug() << "图表坐标鼠标放大位置：" << curPointChart;//event->pos();
        // qDebug() << "图表转视图坐标鼠标放大位置：" << ui->chartView->chart()->mapToPosition(curPointChart, curSeries);//event->pos();
        if (zoomCursorView.empty()) zoomCursorView.append(curPointView);
        if (zoomCursorChart.empty()) zoomCursorChart.append(curPointChart);
        if (delta > 0) {
            ui->chartView->chart()->zoom(0.95);
        } else {
            ui->chartView->chart()->zoom(1.05);
        }
        //第一次的图表位置转为场景位置
        QPointF firstPointScene = ui->chartView->chart()->mapToPosition(zoomCursorChart[0], curSeries);//  curSeries->at(0)
        QPoint firstPointView = ui->chartView->mapFromScene(firstPointScene);
        qDebug() << "缩放前图表转视图坐标鼠标放大位置：" << firstPointView;//event->pos();
        //移动图表
        // ui->chartView->chart()->scroll(-(firstPointView.x()-curPointView.x()) / 1., //1、曲线移动方向和图表坐标系的移动方向相反，2、图表坐标系的y轴向上而chartview组件的坐标系y轴向下
        //                        (firstPointView.y()-curPointView.y()) / 1.);
        ui->chartView->chart()->scroll(-(curPointView.x()-firstPointView.x()) / 1., //1、曲线移动方向和图表坐标系的移动方向相反，2、图表坐标系的y轴向上而chartview组件的坐标系y轴向下
                               (curPointView.y()-firstPointView.y()) / 1.);
        //更新参数面板坐标轴范围
        ui->minDSpinBox->setValue(curAxis->min());
        ui->maxDSpinBox->setValue(curAxis->max());
        //更新绘图(为了画辅助线不缺失)
        // ui->chartView->update();

    }

}
//###############################
//事件
//###############################
void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Meta) {
        isKeyPressed = true;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Meta) {
        isKeyPressed = false;
        zoomCursorView.clear();
        zoomCursorChart.clear();
    }
    QMainWindow::keyReleaseEvent(event);
}



//###########################
//事件过滤器
//###########################
/*
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    qDebug() << "事件对象:" << obj->objectName();
    qDebug() << "事件类型:" << event->type();
    if (obj->objectName() == "chartView") {
        QChart* chart = ui->chartView->chart();
        if (event->type() == QEvent::MouseButtonDblClick) {
            mouseID = 3;
            chart->zoomReset();
            return true;
        } else if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Control) {
                keyID = 0;
                return true;
            }
        } else if (event->type() == QEvent::Wheel && keyID == 0) {
            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            if (wheelEvent->angleDelta().y() > 0){
                // qDebug() << "Zoom In";
                chart->zoom(0.95);
            }else{
                // qDebug() << "Zoom Out";
                chart->zoom(1.05);
            }
            return true;
        } else if (event->type() == QEvent::MouseButtonPress) {
            mouseID = 0;
            QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
            pressedPoint = mouseEvent->pos();
            return true;
        } else if (event->type() == QEvent::MouseMove) {
            // eventID = 1;
            if (mouseID == 0) {
                QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
                chart->scroll(-(mouseEvent->pos().x() - pressedPoint.x()) / 10.,
                               (mouseEvent->pos().y() - pressedPoint.y()) / 10.);
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            mouseID = 2;
            return true;
        } else if (event->type() == QEvent::KeyRelease) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Control) {
                keyID = 1;
                return true;
            }
        }
    }
    return false;
}
*/

//########################
//辅助
//######################
void MainWindow::updateXYGuideLine(QPointF curPoint, bool state) {
    // qDebug() << "state: " << state;
    ui->chartView->hoverState = state;
    if (state) {
        if (!curSeries->points().empty()) {
            QVector<qreal> axisRanges = getAxisRanges();
            QVector<QPointF> xGuideList, yGuideList;
            qreal tempX = curPoint.x();
            qreal tempY = curPoint.y();
            xGuideList.append(QPointF(tempX, axisRanges[2]));
            xGuideList.append(QPointF(tempX, tempY));
            yGuideList.append(QPointF(axisRanges[0], tempY));
            yGuideList.append(QPointF(tempX, tempY));
            xGuideSeries->replace(xGuideList);
            yGuideSeries->replace(yGuideList);
            qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
            qDebug() << "图表坐标数据点: " << curPoint;
            qDebug() << "图表坐标x轴上点: " << xGuideList[0];
            qDebug() << "图表坐标y轴上点: " << yGuideList[0];
            qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
            xGuideList.clear();
            yGuideList.clear();
            //----------------------------------------------------
            QPointF dataPosView = ui->chartView->chart()->mapToPosition(curPoint, curSeries);
            qDebug() << "+++++++++++++++++++++++++++++++++++++++";
            qDebug() << "图表下数据点：" << curPoint;
            qDebug() << "视图下数据点：" << dataPosView;
            //----------------------------------------------------------------
            if (ui->chartView->autoLine.size() <= 3) {
                ui->chartView->autoLine.append(dataPosView);
            } else {
                ui->chartView->autoLine[0] = dataPosView;
            }
            QPointF pos(curPoint.x(), axisRanges[2]);
            QPointF xVerticalView = ui->chartView->chart()->mapToPosition(pos);
            // xVerticalView = ui->chartView->mapFromScene(xVerticalView);
            if (ui->chartView->autoLine.size() <= 3) {
                ui->chartView->autoLine.append(xVerticalView);
            } else {
                ui->chartView->autoLine[1] = xVerticalView;
            }
            qDebug() << "图表下x轴上的点：" << pos;
            qDebug() << "视图下x轴上的点：" << xVerticalView;
            //----------------------------------------------------------------
            pos.setX(axisRanges[0]);
            pos.setY(curPoint.y());
            QPointF yHorizontalView = ui->chartView->chart()->mapToPosition(pos, curSeries);
            if (ui->chartView->autoLine.size() <= 3) {
                ui->chartView->autoLine.append(yHorizontalView);
            } else {
                ui->chartView->autoLine[2] = yHorizontalView;
            }
            qDebug() << "图表下y轴上的点：" << pos;
            qDebug() << "视图下y轴上的点：" << yHorizontalView;
            //----------------------------------------------------------------

        }
    } else {
        ui->chartView->autoLine.clear();
        xGuideSeries->clear();
        yGuideSeries->clear();
    }
    //更新绘画
    // ui->chartView->update();

}

QVector<qreal> MainWindow::getAxisRanges() {
    QChart* chart = ui->chartView->chart();
    QList<QAbstractAxis*> axesX, axesY;
    axesX = chart->axes(Qt::Horizontal);
    axesY = chart->axes(Qt::Vertical);
    QValueAxis* curAxisX = (QValueAxis*) axesX[0];
    QValueAxis* curAxisY = (QValueAxis*) axesY[0];
    QVector<qreal> ranges = {curAxisX->min(), curAxisX->max(), curAxisY->min(), curAxisY->max()};
    return ranges;
}




#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTime>
#include <limits>
#include <QBarSet>
#include <QLineSeries>
#include <QBarSeries>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QPieSeries>
#include <QPieSlice>
#include <QStackedBarSeries>
#include <QPercentBarSeries>
#include <QSplineSeries>
#include <QScatterSeries>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置初始分割比例
    ui->hSplitter->setSizes(QList<int>() << 300 << 300);  // 设置具体像素值
    // 或
    ui->hSplitter->setStretchFactor(0, 1);  // 第一个子部件
    ui->hSplitter->setStretchFactor(1, 3);  // 第二个子部件
    //随机种子
    qsrand(QTime::currentTime().second());
    //模型与视图
    model = new QStandardItemModel(iniDataRowCount, fixedColumnCount,this);
    ui->tableView->setModel(model);
    initData();
    //初始化控件
    ui->holeDSpinBox->setRange(0,1);
    ui->holeDSpinBox->setSingleStep(0.1);
    ui->holeDSpinBox->setValue(0.1);
    ui->pieDSpinBox->setRange(0,1);
    ui->pieDSpinBox->setSingleStep(0.1);
    ui->pieDSpinBox->setValue(0.7);
    //信号与槽
    connect(model, &QStandardItemModel::itemChanged, this, &MainWindow::on_item_changed);
    //初始化柱状图
    initBarChart();
    buildBarChart();
    //初始化饼图
    initPieChart();
    buildPieChart();
    //初始化堆叠图
    initStackChart();
    buildStackChart();
    //初始化百分比图
    initPieChart();
    buildPercentChart();
    //初始化散点图
    initScatterChart();
    buildScatterChart();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_quitAction_triggered() {
    this->close();
}

void MainWindow::initData() {
    QStringList headerList;
    headerList << "姓名" << "数学" << "语文" << "英语" << "平均分";
    model->setHorizontalHeaderLabels(headerList);
    //数据
    for (int row=0; row<model->rowCount(); row++) {
        QString stuName = QString::asprintf("学生%2d", row+1);
        // QStandardItem item(stuName);
        QStandardItem* item = new QStandardItem(stuName);
        item->setTextAlignment(Qt::AlignCenter);
        model->setItem(row, colNoName, item);
        qreal aveValue = 0;
        for (int column=colNoMath; column<=colNoEnglish; column++) {
            qreal score = 50. + qrand() % 50;
            item = new QStandardItem(QString::asprintf("%.0f",score));
            item->setTextAlignment(Qt::AlignCenter);
            model->setItem(row, column, item);
            aveValue += score;
        }
        item = new QStandardItem(QString::asprintf("%.1f",aveValue/3));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(item->flags() & !Qt::ItemIsEditable);
        model->setItem(row, colNoAverage, item);
    }
}

void MainWindow::surveyData() {
    for (int column=colNoMath; column<=colNoAverage; column++) {
        qreal minValue = std::numeric_limits<qreal>::max();
        qreal maxValue = std::numeric_limits<qreal>::min();
        qreal aveValue = 0;
        int cnt60=0, cnt70=0, cnt80=0, cnt90=0, cnt100=0;
        for (int row=0; row<model->rowCount(); row++) {
            double val = model->item(row, column)->text().toDouble();
            if (val > maxValue) maxValue = val;
            if (val < minValue) minValue = val;
            if (val<60) cnt60++;
            else if (val<70) cnt70++;
            else if (val<80) cnt80++;
            else if (val<90) cnt90++;
            else if (val>=90) cnt100++;
            aveValue += val;
        }
        QTreeWidgetItem* item = ui->treeWidget->topLevelItem(0);
        item->setTextAlignment(column, Qt::AlignCenter);
        item->setText(column, QString::number(cnt60));

        item = ui->treeWidget->topLevelItem(1);
        item->setTextAlignment(column, Qt::AlignCenter);
        item->setText(column, QString::number(cnt70));

        item = ui->treeWidget->topLevelItem(2);
        item->setTextAlignment(column, Qt::AlignCenter);
        item->setText(column, QString::number(cnt80));

        item = ui->treeWidget->topLevelItem(3);
        item->setTextAlignment(column, Qt::AlignCenter);
        item->setText(column, QString::number(cnt90));

        item = ui->treeWidget->topLevelItem(4);
        item->setTextAlignment(column, Qt::AlignCenter);
        item->setText(column, QString::number(cnt100));

        item = ui->treeWidget->topLevelItem(5);
        item->setTextAlignment(column, Qt::AlignCenter);
        item->setText(column, QString::asprintf("%.1f", aveValue/model->rowCount()));

        item = ui->treeWidget->topLevelItem(6);
        item->setTextAlignment(column, Qt::AlignCenter);
        item->setText(column, QString::number(maxValue));

        item = ui->treeWidget->topLevelItem(7);
        item->setTextAlignment(column, Qt::AlignCenter);
        item->setText(column, QString::number(minValue));
    }
}

void MainWindow::initBarChart() {
    QChart* chart = new QChart;
    chart->setTitle("Bar Chart展示");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    ui->barChart->setChart(chart);
    ui->barChart->setRenderHint(QPainter::Antialiasing);
    ui->barChart->chart()->legend()->setAlignment(Qt::AlignBottom);
}

void MainWindow::buildBarChart() {
    QChart* chart = ui->barChart->chart();
    //删除原序列
    chart->removeAllSeries();
    //坐标轴
    //删除原坐标轴
    if (chart->axisX() != nullptr) {
        chart->removeAxis(chart->axisX());
    }
    if (chart->axisY() != nullptr) {
        chart->removeAxis(chart->axisY());
    }
    // QValueAxis* axisX = new QValueAxis;
    QValueAxis* axisY = new QValueAxis();
    QStringList categories;
    QBarCategoryAxis* axisBX = new QBarCategoryAxis();
    axisY->setTitleText("分数");
    // axisBX->setCategories(categories);
    //图表添加轴
    chart->addAxis(axisBX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    //数据集
    QBarSet* setMath = new QBarSet(model->horizontalHeaderItem(colNoMath)->text());
    QBarSet* setChinese = new QBarSet(model->horizontalHeaderItem(colNoChinese)->text());
    QBarSet* setEnglish = new QBarSet(model->horizontalHeaderItem(colNoEnglish)->text());
    //序列
    QLineSeries* line = new QLineSeries();
    line->setName(model->horizontalHeaderItem(colNoAverage)->text());
    line->setColor(Qt::red);
    line->setPointsVisible(true);
    QBarSeries* barSeries = new QBarSeries();
    //图表添加序列, 注图表添加序列不能放在序列放在坐标轴的语句之后
    chart->addSeries(barSeries);
    chart->addSeries(line);
    //序列添加数据集
    barSeries->append(setMath);
    barSeries->append(setChinese);
    barSeries->append(setEnglish);
    barSeries->setLabelsVisible(true);

    for (int row=0; row<model->rowCount(); row++) {
        setMath->append( model->item(row, colNoMath)->text().toDouble());
        setChinese->append( model->item(row, colNoChinese)->text().toDouble());
        setEnglish->append( model->item(row, colNoEnglish)->text().toDouble());
        line->append(QPointF(row, model->item(row, colNoAverage)->text().toDouble()));
        categories.append(model->item(row, colNoName)->text());
    }
    axisBX->setCategories(categories);

    //序列放到轴上
    barSeries->attachAxis(axisBX);
    barSeries->attachAxis(axisY);
    line->attachAxis(axisBX);
    line->attachAxis(axisY);
    // chart->setAxisX(axisBX, line);
    // chart->setAxisY(axisY, line);
    // chart->setAxisX(axisBX, barSeries);
    // chart->setAxisY(axisY, barSeries);

    //坐标轴范围
    axisBX->setRange(categories[0], categories[categories.count()-1]);
    axisY->setRange(0, 100);


}

void MainWindow::initPieChart() {
    QChart* chart = new QChart();
    chart->setTitle("Pie Chart展示");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    ui->pieChart->setChart(chart);
    ui->pieChart->setRenderHint(QPainter::Antialiasing);
    ui->pieChart->chart()->legend()->setAlignment(Qt::AlignRight);
    ui->pieChart->chart()->setTheme(QChart::ChartTheme(0));
    surveyData();
}

void MainWindow::buildPieChart() {
    //图表
    QChart* chart = ui->pieChart->chart();
    chart->removeAllSeries();
    if (chart->axisX() != nullptr) {
        chart->removeAxis(chart->axisX());
    }
    if (chart->axisY() != nullptr) {
        chart->removeAxis(chart->axisY());
    }
    ui->pieChart->chart()->setTheme(QChart::ChartTheme(ui->pieThemeComboBox->currentIndex()));
    chart->setTitle("Pie Chart-------"+ui->pieDataComboBox->currentText());

    //序列
    QPieSeries* pieSeries = new QPieSeries();
    chart->addSeries(pieSeries);
    //中间圆孔的数值不超过1, 否则扇形画不出来
    pieSeries->setHoleSize(ui->holeDSpinBox->value());
    //pie大小数值不超过1, 否则标签显示不全
    pieSeries->setPieSize(ui->pieDSpinBox->value());
    int colNo = ui->pieDataComboBox->currentIndex()+1;
    for (int i=0; i<5; i++) {
        QTreeWidgetItem* item = ui->treeWidget->topLevelItem(i);
        pieSeries->append(item->text(0), item->text(colNo).toDouble());
    }

    pieSeries->setLabelsVisible(true);
    //块
    QPieSlice* pieSlice;
    for (int i=0; i<pieSeries->slices().size(); i++) {
        pieSlice = pieSeries->slices().at(i);
        pieSlice->setLabel(pieSlice->label()+QString::asprintf("%.0f人，%.1f\%", pieSlice->value(), pieSlice->percentage()*100));
        // pieSlice->setLabelVisible(true);
        //分离图例和标签显示的内容
        //块的信号与槽
        connect(pieSlice, &QPieSlice::hovered, this, &MainWindow::onSliceHighLight);
    }
}

void MainWindow::initStackChart() {
    QChart* chart = new QChart();
    chart->setTitle("Stack Chart展示");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    ui->pieChart->setChart(chart);
    ui->pieChart->setRenderHint(QPainter::Antialiasing);
    ui->pieChart->chart()->legend()->setAlignment(Qt::AlignRight);
    // ui->pieChart->chart()->setTheme(QChart::ChartTheme(0));
}

void MainWindow::buildStackChart() {
    QChart* chart = ui->stackChart->chart();
    //删除原序列
    chart->removeAllSeries();
    //坐标轴
    //删除原坐标轴
    if (chart->axisX() != nullptr) {
        chart->removeAxis(chart->axisX());
    }
    if (chart->axisY() != nullptr) {
        chart->removeAxis(chart->axisY());
    }
    // QValueAxis* axisX = new QValueAxis;
    QValueAxis* axisY = new QValueAxis();
    QStringList categories;
    QBarCategoryAxis* axisBX = new QBarCategoryAxis();
    axisY->setTitleText("总分");
    // axisBX->setCategories(categories);
    //图表添加轴
    chart->addAxis(axisBX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    //数据集
    QBarSet* setMath = new QBarSet(model->horizontalHeaderItem(colNoMath)->text());
    QBarSet* setChinese = new QBarSet(model->horizontalHeaderItem(colNoChinese)->text());
    QBarSet* setEnglish = new QBarSet(model->horizontalHeaderItem(colNoEnglish)->text());
    //序列
    // QLineSeries* line = new QLineSeries();
    // line->setName(model->horizontalHeaderItem(colNoAverage)->text());
    // line->setColor(Qt::red);
    // line->setPointsVisible(true);
    QStackedBarSeries* barSeries = new QStackedBarSeries();
    //图表添加序列, 注图表添加序列不能放在序列放在坐标轴的语句之后
    chart->addSeries(barSeries);
    // chart->addSeries(line);
    //序列添加数据集
    barSeries->append(setMath);
    barSeries->append(setChinese);
    barSeries->append(setEnglish);
    barSeries->setLabelsVisible(true);

    for (int row=0; row<model->rowCount(); row++) {
        setMath->append( model->item(row, colNoMath)->text().toDouble());
        setChinese->append( model->item(row, colNoChinese)->text().toDouble());
        setEnglish->append( model->item(row, colNoEnglish)->text().toDouble());
        // line->append(QPointF(row, model->item(row, colNoAverage)->text().toDouble()));
        categories.append(model->item(row, colNoName)->text());
    }
    axisBX->setCategories(categories);

    //序列放到轴上
    barSeries->attachAxis(axisBX);
    barSeries->attachAxis(axisY);
    // line->attachAxis(axisBX);
    // line->attachAxis(axisY);
    // chart->setAxisX(axisBX, line);
    // chart->setAxisY(axisY, line);
    // chart->setAxisX(axisBX, barSeries);
    // chart->setAxisY(axisY, barSeries);

    //坐标轴范围
    axisBX->setRange(categories[0], categories[categories.count()-1]);
    axisY->setRange(0, 300);
}

void MainWindow::initPercentChart() {
    QChart* chart = new QChart();
    chart->setTitle("Percent Chart展示");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    ui->percentChart->setChart(chart);
    ui->percentChart->setRenderHint(QPainter::Antialiasing);
    ui->percentChart->chart()->legend()->setAlignment(Qt::AlignRight);
    // ui->pieChart->chart()->setTheme(QChart::ChartTheme(0));
}

void MainWindow::buildPercentChart() {
    QChart* chart = ui->percentChart->chart();
    //删除原序列
    chart->removeAllSeries();
    //坐标轴
    //删除原坐标轴
    if (chart->axisX() != nullptr) {
        chart->removeAxis(chart->axisX());
    }
    if (chart->axisY() != nullptr) {
        chart->removeAxis(chart->axisY());
    }
    // QValueAxis* axisX = new QValueAxis;
    QValueAxis* axisY = new QValueAxis();
    QStringList categories;
    QBarCategoryAxis* axisBX = new QBarCategoryAxis();
    axisY->setTitleText("各科人数占比");
    // axisBX->setCategories(categories);
    //图表添加轴
    chart->addAxis(axisBX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    //数据集
    QTreeWidget* tree = ui->treeWidget;
    QBarSet* setMath = new QBarSet(tree->headerItem()->text(colNoMath));
    QBarSet* setChinese = new QBarSet(tree->headerItem()->text(colNoChinese));
    QBarSet* setEnglish = new QBarSet(tree->headerItem()->text(colNoEnglish));
    //序列
    // QLineSeries* line = new QLineSeries();
    // line->setName(model->horizontalHeaderItem(colNoAverage)->text());
    // line->setColor(Qt::red);
    // line->setPointsVisible(true);
    QStackedBarSeries* barSeries = new QStackedBarSeries();
    //图表添加序列, 注图表添加序列不能放在序列放在坐标轴的语句之后
    chart->addSeries(barSeries);
    // chart->addSeries(line);
    //序列添加数据集
    barSeries->append(setMath);
    barSeries->append(setChinese);
    barSeries->append(setEnglish);
    barSeries->setLabelsVisible(true);
    barSeries->setLabelsFormat("@value%");
    // barSeries->setLabelsFormat("@.1f%");
    // barSeries->setLabelsFormat("%.1f%");
    // barSeries->setLabelsPrecision(1);
    int count = tree->topLevelItemCount() - 3;
    for (int row=0; row<count; row++) {
        QTreeWidgetItem* item = tree->topLevelItem(row);
        qreal totValue = item->text(colNoMath).toDouble() + item->text(colNoChinese).toDouble() + item->text(colNoEnglish).toDouble();
        qreal val;
        val = item->text(colNoMath).toDouble()/totValue*100;
        setMath->append(QString::asprintf("%.1f", val).toDouble());
        qDebug() << "math:"<< QString::asprintf("%.1f", val).toDouble();
        val = item->text(colNoChinese).toDouble()/totValue*100;
        setChinese->append(QString::asprintf("%.1f", val).toDouble());
        qDebug() << "chinese:" << QString::asprintf("%.1f", val).toDouble();
        val = item->text(colNoEnglish).toDouble()/totValue*100;
        setEnglish->append(QString::asprintf("%.1f", val).toDouble());
        qDebug() << "english:" << QString::asprintf("%.1f", val).toDouble();
        // line->append(QPointF(row, model->item(row, colNoAverage)->text().toDouble()));
        categories.append(item->text(colNoName));
    }
    axisBX->setCategories(categories);

    //序列放到轴上
    barSeries->attachAxis(axisBX);
    barSeries->attachAxis(axisY);
    // line->attachAxis(axisBX);
    // line->attachAxis(axisY);
    // chart->setAxisX(axisBX, line);
    // chart->setAxisY(axisY, line);
    // chart->setAxisX(axisBX, barSeries);
    // chart->setAxisY(axisY, barSeries);

    //坐标轴范围
    axisBX->setRange(categories[0], categories[categories.count()-1]);
    axisY->setRange(0, 100);
    axisY->setLabelFormat("%.1f%");
}

void MainWindow::initScatterChart() {
    QChart* chart = new QChart();
    chart->setTitle("Scatter Chart展示");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    ui->scatterChart->setChart(chart);
    ui->scatterChart->setRenderHint(QPainter::Antialiasing);
    ui->scatterChart->chart()->legend()->setAlignment(Qt::AlignRight);
}

void MainWindow::buildScatterChart() {
     QChart* chart = ui->scatterChart->chart();
    //删除原序列
    chart->removeAllSeries();
    //坐标轴
    //删除原坐标轴
    if (chart->axisX() != nullptr) {
        chart->removeAxis(chart->axisX());
    }
    if (chart->axisY() != nullptr) {
        chart->removeAxis(chart->axisY());
    }
    QValueAxis* axisX = new QValueAxis;
    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("y轴");
    axisX->setTitleText("x轴");
    // axisBX->setCategories(categories);
    //图表添加轴
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    //序列
    QSplineSeries* line = new QSplineSeries();
    QPen pen(Qt::red);
    pen.setWidth(2);
    line->setName("spline");
    // line->setPointsVisible(true);
    line->setPen(pen);
    QScatterSeries* scatterSeries = new QScatterSeries();
    scatterSeries->setName("scatter");
    // scatterSeries->setPointsVisible(true);
    scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatterSeries->setBorderColor(Qt::black);
    scatterSeries->setBrush(Qt::blue);
    scatterSeries->setMarkerSize(12);
    //图表添加序列, 注图表添加序列不能放在序列放在坐标轴的语句之后
    chart->addSeries(scatterSeries);
    chart->addSeries(line);
    //序列添加数据集
    for (int row=0; row<iniDataRowCount; row++) {
        int x = qrand() % 20;
        int y = qrand() % 20;
        scatterSeries->append(x,y);
        line->append(x,y);
    }

    //序列放到轴上
    scatterSeries->attachAxis(axisX);
    scatterSeries->attachAxis(axisY);
    line->attachAxis(axisX);
    line->attachAxis(axisY);


    //坐标轴范围
    axisX->setRange(0, 20);
    axisY->setRange(0, 20);
    axisY->setLabelFormat("%.1f");
    axisX->setLabelFormat("%.1f");
}

void MainWindow::on_generateAction_triggered() {
    initData();
}

void MainWindow::on_statisticAction_triggered() {
    surveyData();
}

void MainWindow::on_item_changed(QStandardItem* item) {
    int row = item->row();
    int col = item->column();
    if (col>=colNoMath && col<=colNoEnglish) {
        qreal val;// = item->text().toDouble();
        QStandardItem* item;
        qreal aveValue = 0;
        for (int column=colNoMath; column<=colNoEnglish; column++) {
            val = model->item(row, column)->text().toDouble();
            aveValue += val;
        }
        item = new QStandardItem(QString::asprintf("%.0f",aveValue/3));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(item->flags() & !Qt::ItemIsEditable);
        model->setItem(row, colNoAverage, item);
    }
}

void MainWindow::on_barBtn_clicked() {
    buildBarChart();
}

void MainWindow::on_pieBtn_clicked() {
    buildPieChart();
}

void MainWindow::on_percentBtn_clicked() {
    buildPercentChart();
}

void MainWindow::onSliceHighLight(bool show) {
    //谁发送的信号
    QPieSlice* pieSlice = (QPieSlice*)sender();
    pieSlice->setExploded(show);

}

void MainWindow::on_holeDSpinBox_valueChanged(double value) {
    if (!ui->pieChart->chart()->series().empty()) {
        QPieSeries* pieSeries = dynamic_cast<QPieSeries *>(ui->pieChart->chart()->series()[0]);
        pieSeries->setHoleSize(ui->holeDSpinBox->value());
    }

}

void MainWindow::on_pieDSpinBox_valueChanged(double value) {
    if (!ui->pieChart->chart()->series().empty()) {
        QPieSeries* pieSeries = dynamic_cast<QPieSeries *>(ui->pieChart->chart()->series()[0]);
        pieSeries->setPieSize(ui->pieDSpinBox->value());
    }
}

void MainWindow::on_pieThemeComboBox_currentIndexChanged(int index) {
    ui->pieChart->chart()->setTheme(QChart::ChartTheme(index));
}
void MainWindow::on_pieDataComboBox_currentIndexChanged(int index) {
    // QPieSeries* pieSeries = dynamic_cast<QPieSeries *>(ui->pieChart->chart()->series()[0]);
    // ui->pieChart->chart()->setTitle("Pie Chart-------"+ui->pieDataComboBox->currentText());
    // pieSeries->
    buildPieChart();
}

void MainWindow::on_scatterBtn_clicked() {
    buildScatterChart();
}


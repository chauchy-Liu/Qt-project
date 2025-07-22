#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qchartglobal.h>
#include <QMainWindow>
#include <QChart>
#include <QStandardItemModel>
#include <QTableWidgetItem>

#define fixedColumnCount 5
#define iniDataRowCount 10
#define colNoName 0
#define colNoMath 1
#define colNoChinese 2
#define colNoEnglish 3
#define colNoAverage 4

using namespace QtCharts;

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

private slots:
    void on_quitAction_triggered();
    void on_generateAction_triggered();
    void on_statisticAction_triggered();
    void on_item_changed(QStandardItem* item);
    void on_barBtn_clicked();
    void on_pieBtn_clicked();

    void on_percentBtn_clicked();

    void onSliceHighLight(bool show);
    void on_holeDSpinBox_valueChanged(double value);

    void on_pieDSpinBox_valueChanged(double value);

    void on_pieThemeComboBox_currentIndexChanged(int index);

    void on_pieDataComboBox_currentIndexChanged(int index);

    void on_scatterBtn_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;

    void initData();
    void surveyData();
    void initBarChart();
    void buildBarChart();
    void initPieChart();
    void buildPieChart();
    void initStackChart();
    void buildStackChart();
    void initPercentChart();
    void buildPercentChart();
    void initScatterChart();
    void buildScatterChart();


};
#endif // MAINWINDOW_H

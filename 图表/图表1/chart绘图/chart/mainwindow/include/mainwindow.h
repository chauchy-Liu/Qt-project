#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CustomChartView.h"
// #include <QtCharts>
#include <QToolTip>
#include <QValueAxis>

// using namespace QtCharts;//或者用宏 Qt_CHARTS_USE_NAMESPACE
// Qt_CHARTS_USE_NAMESPACE

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
    QLineSeries *curSeries;
    QValueAxis *curAxis;
    QMap<QString, bool> cursorDataVisible;
    QMap<QString, bool> dataAutoLine;
    bool isMousePressed = false;
    bool isKeyPressed = false;
    QPoint pressedPoint;
    QLineSeries* xGuideSeries;    //鼠标悬浮位置点的x轴辅助线 QSplineSeries
    QLineSeries* yGuideSeries;    //鼠标悬浮位置点的y轴辅助线 QSplineSeries
    QList<QPoint> zoomCursorView; //放大时鼠标在视图中的位置
    QList<QPointF> zoomCursorChart; //放大时鼠标在图表中的位置
    QTimer *paintTimer;
    QLabel* xyCoord;

    void createChart();
    void prepareData();
    void updateBoxFromChart();
    QVector<qreal> getAxisRanges();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    // void mouseMoveEvent(QMouseEvent *event) override;
    // void mouseReleaseEvent(QMouseEvent *event) override;
    // void mouseDoubleClickEvent(QMouseEvent *event) override;
    // void wheelEvent(QWheelEvent *event) override;

    // bool eventFilter(QObject *obj, QEvent *event) override;
    // void paintEvent(QPaintEvent *event) override;



private slots:
    void updateXYGuideLine(QPointF curPoint, bool state);
    void theSlotMouseEvent(int eventId, QMouseEvent *event);
    // void theSlotKeyEvent(int eventId, QKeyEvent *event);
    void theSlotWheelEvent(QWheelEvent *event);
    void on_quitActionTriggered();
    void on_drawActionTriggered();
    void on_zoominActionTriggered();
    void on_zoomoutActionTriggered();
    void on_zoomActionTriggered();
    void onLegendMarkClicked();

    void on_btnSetTitle_clicked();

    void on_btnSetTitleFont_clicked();

    void on_rdBtn1_clicked();

    void on_rdBtn2_clicked();

    void on_rdBtn3_clicked();

    void on_rdBtn4_clicked();

    void on_checkBox2_clicked();

    void on_checkBox1_clicked();

    void on_btnFont_clicked();

    void on_btnLabelColor_clicked();

    void on_sideBtn_clicked();

    void on_fontColorBtn2_clicked();

    void on_labelColorBtn2_clicked();

    void on_comboBox1_currentIndexChanged(int index);

    void on_comboBox2_currentIndexChanged(int index);

    void on_curveTypeRdBtn1_clicked();

    void on_curveTypeRdBtn2_clicked();

    void on_btnCurveName_clicked();

    void on_checkBox3_clicked(bool checked);

    void on_checkBox4_clicked(bool checked);

    void on_curveColorBtn_clicked();

    void on_penBtn_clicked();

    void on_transparencyHSlider_valueChanged(int value);

    void on_checkBox5_clicked(bool checked);

    void on_labelColorBtn_clicked();

    void on_labelFontBtn_clicked();

    void on_labelFormatRdBtn1_clicked();

    void on_labelFormatRdBtn2_clicked();

    void on_checkBox11_clicked(bool checked);

    void on_checkBox12_clicked(bool checked);

    void on_hoverSeries_cursor(QPointF point, bool state);

    void on_xRdBtn_clicked(bool checked);

    void on_yRdBtn_clicked(bool checked);

    void on_rangeBtn_clicked();

    void on_checkBox6_clicked(bool checked);

    void on_titleBtn_clicked();

    void on_fontBtn1_clicked();

    void on_checkBox7_clicked(bool checked);

    void on_labelFormatBtn_clicked();

    void on_fontColorBtn_clicked();

    void on_fontBtn2_clicked();

    void on_checkBox8_clicked(bool checked);

    void on_gridColorBtn_clicked();

    void on_gridPenBtn_clicked();

    void on_tickSpinBox_valueChanged(int value);

    void on_checkBox9_clicked(bool checked);

    void on_tickColorBtn_clicked();

    void on_tickPenBtn_clicked();

    void on_minorTickSpinBox_valueChanged(int value);

    void on_checkBox10_clicked(bool checked);

    void on_minorTickPenBtn_clicked();

    void on_minorTickColorBtn_clicked();

};
#endif // MAINWINDOW_H

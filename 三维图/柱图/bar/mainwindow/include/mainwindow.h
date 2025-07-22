#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDataVisualization>

// using namespace QtDataVisualization;

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
    Q3DBars* barChart;

    void initGraph3D();

private slots:
    void on_sightComboBox_activated(int index);

    void on_hSpinSlider_valueChanged(int value);

    void on_vSpinSlider_valueChanged(int value);

    void on_scaleSlider_valueChanged(int value);

    void on_themeComboBox_currentIndexChanged(int index);

    void on_barComboBox_currentIndexChanged(int index);

    void on_selectComboBox_currentIndexChanged(int index);

    void on_fontSpinBox_valueChanged(int value);

    void on_itemBtn_clicked();

    void on_backgroundCheckBox_clicked(bool checked);

    void on_gridCheckBox_clicked(bool checked);

    void on_smoothCheckBox_clicked(bool checked);

    void on_invertShadeCheckBox_clicked(bool checked);

    void on_invertValueCheckBox_clicked(bool checked);

    void on_itemLabelCheckBox_clicked(bool checked);

    void axisTitleCheckBox_clicked(bool checked);

    void on_axisLabelCheckBox_clicked(bool checked);
};
#endif // MAINWINDOW_H

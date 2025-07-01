#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QNetworkReply>
#include <QMouseEvent>
#include "mapUtil/CityCodeUtils.h"
#include "day.h"
#include "style.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Day days[7];
    QList<QLabel*> mDateList;
    QList<QLabel*> mWeekList;
    QList<QLabel*> mIconList;
    QList<QLabel*> mWeatherTypList;
    QList<QLabel*> mAirqList;
    QList<QLabel*> mFxList;
    QList<QLabel*> mFlList;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    // void paintEvent(QPaintEvent *event) override;
private:
    Ui::MainWindow *ui;
    QMenu* menu;
    QAction* actionClose;
    QPoint moveOffSet;
    QNetworkReply* response;
    QString urlStr;// = "http://gfeljm.tianqiapi.com/api?unescape=1&version=v9&appid=21676181&appsecret=CfbYCU9k";
    QNetworkAccessManager* manager;
    bool firstFocus;
    QMap<QString, QString> weatherIconMap;

    CityCodeUtils cityCodeUtils;


    void parseWeatherJson(QByteArray data);

    void on_searchLabel_clicked();

    void updateUIFace();

    void drawTempLine(Day *days, QString key, QWidget *parent, Style styleMap);

private slots:
    void onHttpReply(QNetworkReply *reply);

};
#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonArray>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setLayout(ui->wholeVL);
    // ui->widget2->setLayout(ui->widget2);
    //固定应用窗体尺寸
    setFixedSize(432, 784);
    //设置右键后的弹出菜单
    menu = new QMenu(this);
    actionClose = new QAction(QIcon(":weather/images/close.png"), tr("退出"), this);
    menu->addAction(actionClose);
    //设置右键菜单颜色
    menu->setStyleSheet("QMenu::item {color:white; background-color:gray}");
    //设置搜索label的事件过滤器
    ui->searchLabel->installEventFilter(this);
    ui->searchLineEdit->installEventFilter(this);
    ui->plotMaxWidget->installEventFilter(this);
    ui->plotMinWidget->installEventFilter(this);
    //网络请求
    //QNetworkAccessManager对象发起请求
    manager = new QNetworkAccessManager(this);
    //请求信息
    urlStr = "http://gfeljm.tianqiapi.com/api?unescape=1&version=v9&appid=21676181&appsecret=CfbYCU9k";
    QUrl urlUrl = QUrl(urlStr);
    QNetworkRequest request(urlUrl);
    request.setRawHeader("Content-Type", "application/json; charset=utf-8");

    // QNetworkRequest request(QUrl("http://gfeljm.tianqiap.com/api?unescape=1&version=v9&appid=21676181&appsecret=CfbYCU9k"));
    //接收信息
    response = manager->get(request);

    //
    mWeekList << ui->timeUpLabel1 <<  ui->timeUpLabel2 <<   ui->timeUpLabel3
        <<  ui->timeUpLabel4 <<  ui->timeUpLabel5 <<  ui->timeUpLabel6;
    mDateList << ui->timeBottomLabel1 <<  ui->timeBottomLabel2 <<  ui->timeBottomLabel3
        <<  ui->timeBottomLabel4 <<  ui->timeBottomLabel5 <<  ui->timeBottomLabel6;
    mIconList << ui->picLabel1 <<  ui->picLabel2 <<  ui->picLabel3 <<  ui->picLabel4
        <<  ui->picLabel5 <<  ui->picLabel6;
    mWeatherTypList << ui->picTextLabel1 <<  ui->picTextLabel2 <<  ui->picTextLabel3
        <<  ui->picTextLabel4 <<  ui->picTextLabel5 <<  ui->picTextLabel6;
    mAirqList << ui->airLabel1 <<  ui->airLabel2 <<  ui->airLabel3 <<  ui->airLabel4
        <<  ui->airLabel5 <<  ui->airLabel6;
    mFxList << ui->windUpLabel1 <<  ui->windUpLabel2 <<  ui->windUpLabel3 <<  ui->windUpLabel4
        <<  ui->windUpLabel5 <<  ui->windUpLabel6;
    mFlList << ui->windBottomLabel1 <<  ui->windBottomLabel2 <<  ui->windBottomLabel3 <<  ui->windBottomLabel4
        <<  ui->windBottomLabel5 <<  ui->windBottomLabel6;

    //天气图片映射
    weatherIconMap.insert("晴", ":/图标素材/rose/0.png");
    weatherIconMap.insert("暴雪", ":/图标素材/pitaya/17.png");
    weatherIconMap.insert("暴雨", ":/图标素材/rose/12.png");
    weatherIconMap.insert("暴雨到大暴雨", ":/图标素材/pitaya/21.png");
    weatherIconMap.insert("大暴雨", ":/图标素材/pitaya/22.png");
    weatherIconMap.insert("大暴雨到特大暴雨", ":/图标素材/pitaya/23.png");
    weatherIconMap.insert("大暴雪", ":/图标素材/yahoo/xue.png");
    weatherIconMap.insert("大雪", ":/图标素材/pitaya/16.png");
    weatherIconMap.insert("大雨", ":/图标素材/rose/11.png");
    weatherIconMap.insert("冻雨", ":/图标素材/rose/19.png");
    weatherIconMap.insert("多云", ":/图标素材/rose/1.png");
    weatherIconMap.insert("浮沉", ":/图标素材/rose/31.png");
    weatherIconMap.insert("雷阵雨", ":/图标素材/rose/4.png");
    weatherIconMap.insert("雷阵雨伴有冰雹", ":/图标素材/rose/5.png");
    weatherIconMap.insert("霾", ":/图标素材/pitaya/18.png");
    weatherIconMap.insert("小到中雪", ":/图标素材/rose/15.png");
    weatherIconMap.insert("小到中雨", ":/图标素材/rose/8.png");
    weatherIconMap.insert("强沙尘暴", ":/图标素材/cherry/shachen.png");
    weatherIconMap.insert("沙尘暴", ":/图标素材/yahoo/shachen.png");
    weatherIconMap.insert("特大暴雨", ":/图标素材/rose/25.png");
    weatherIconMap.insert("雾", ":/图标素材/pitaya/wu.png");
    weatherIconMap.insert("小雪", ":/图标素材/rose/14.png");
    weatherIconMap.insert("小雨", ":/图标素材/rose/7.png");
    weatherIconMap.insert("雪", ":/图标素材/pitaya/14.png");
    weatherIconMap.insert("扬沙", ":/图标素材/cucumber/shachen.png");
    weatherIconMap.insert("阴", ":/图标素材/rose/2.png");
    weatherIconMap.insert("雨", ":/图标素材/rose/6.png");
    weatherIconMap.insert("雨夹雪", ":/图标素材/pitaya/6.png");
    weatherIconMap.insert("阵雪", ":/图标素材/apple/xue.png");
    weatherIconMap.insert("阵雨", ":/图标素材/rose/3.png");
    weatherIconMap.insert("中到大雪", ":/图标素材/rose/17.png");
    weatherIconMap.insert("中到到大雨", ":/图标素材/rose/10.png");
    weatherIconMap.insert("中雪", ":/图标素材/rose/16.png");
    weatherIconMap.insert("中雨", ":/图标素材/rose/9.png");

    //信号与槽
    connect(menu, &QMenu::triggered, [=](){this->close();});
    // connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) {
    //     qDebug() << "==============manager finish!==============" ;//<< response->errorString();
    // });
    // connect(response, &QNetworkReply::finished, this, &MainWindow::onHttpReply);
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::onHttpReply);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    QMainWindow::mousePressEvent(event);
    if (event->button() == Qt::RightButton) {
        qDebug() << "right mouse clicked";
        menu->exec(QCursor::pos());
    }
    //鼠标当前位置，窗口当前位置，窗口新位置
    if (event->button() == Qt::LeftButton) {
        qDebug() << "left mouse clicked";
        qDebug() << "鼠标在应用窗体中的位置："<<event->pos();
        qDebug() << "鼠标在屏幕中的位置："<<event->globalPos(); //globalPosition();
        qDebug() << "应用窗口当前的位置(应有左上角坐标)："<<this->pos();
        qDebug() << "应用窗口新的位置："<<event->globalPos()-event->pos();
        moveOffSet = event->globalPos()-this->pos();//event->pos(); //event->globalPosition().toPoint()-event->pos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    QMainWindow::mouseMoveEvent(event);
    //鼠标拖动后窗口的新位置
    this->move(event->globalPos()-moveOffSet);//moveOffSet
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    // qDebug() << "使用⌚️过滤器增加事件触发信号eventFilter:" << obj;
    // qDebug() << "使用⌚️过滤器增加事件触发信号type:" << event->type();
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton && obj->objectName() == "searchLabel") {
            on_searchLabel_clicked();
            return true;
        } else if (mouseEvent->button()  == Qt::LeftButton  && obj->objectName() == "searchLineEdit") {
            if (firstFocus == true) {
                ui->searchLineEdit->clear();
                firstFocus = false;
                return true;
            }
        }
    } else if (event->type() == QEvent::KeyPress && obj->objectName() == "searchLineEdit") {
        QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return) {
            on_searchLabel_clicked();
            return true;
        }
    } else if (event->type() == QEvent::Enter && obj->objectName() == "searchLineEdit") {
        QEnterEvent* focusEvent = static_cast<QEnterEvent *>(event);
        QPointF pos = focusEvent->pos();
        QPointF globPos = focusEvent->globalPos();
        QRect lineEditRect = ui->searchLineEdit->frameGeometry();
        if (pos.x()+lineEditRect.x() > lineEditRect.x() && pos.y()+lineEditRect.y() > lineEditRect.y() && pos.x()+lineEditRect.x() < lineEditRect.x()+lineEditRect.width() && pos.y()+lineEditRect.y() < lineEditRect.y()+lineEditRect.height()) {
            firstFocus = true;
        }/*else {
            firstFocus = false;
        }*/
        return true;
    }else if (event->type() == QEvent::Paint && obj->objectName() == "plotMaxWidget") {
        //更新温度曲线
        Style styleMap;
        styleMap.color = QColor(248,176,12, 200);
        styleMap.brush = QBrush(styleMap.color);
        // styleMap.brush.setStyle(Qt::SolidPattern);
        styleMap.frameScaled = 0.6;
        styleMap.marginBottom = 10;
        drawTempLine(days, "mTempHigh", ui->plotMaxWidget, styleMap);
        return true;
    }else if (event->type() == QEvent::Paint && obj->objectName() == "plotMinWidget") {
        //更新温度曲线
        Style styleMap;
        styleMap.color = QColor(95,155,255, 200);
        styleMap.brush = QBrush(styleMap.color);
        // styleMap.brush.setStyle(Qt::SolidPattern);
        styleMap.frameScaled = 0.6;
        styleMap.marginBottom = 10;
        drawTempLine(days, "mTempLow", ui->plotMinWidget, styleMap);
        return true;
    }

    // return MainWindow::eventFilter(obj, event);
    return false;
}

// void MainWindow::paintEvent(QPaintEvent *event) {
//     QMainWindow::paintEvent(event);
//     QPainter painter(this);
// }


void MainWindow::onHttpReply(QNetworkReply* reply) {
    qDebug() << "处理请求URL：" << reply->url();
    QVariant status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);//response->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "http reply:" << reply->error() << ", status:" << status.toInt();
    if (reply->error() == QNetworkReply::NoError && status.toInt() == 200) {
        QByteArray data = reply->readAll();
        QString dataStr = QString::fromUtf8(data);
        //转成字符串
        // qDebug() << "data:" << QString::fromUtf8(data);
        //解析json
        parseWeatherJson(data);
        //更新UI界面
        updateUIFace();
        // parseWeatherJson(dataStr);
    }else {
        QMessageBox msg;
        msg.setWindowTitle("网络错误");
        msg.setText(QString("当前网络状态为：%1").arg(status.toInt()));
        msg.setIconPixmap(QPixmap(":weather/images/alert.png").scaled(20,20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        msg.setStyleSheet("QPushButton {color:white; display:grid; justify-content: center; place-items: center;}");
        msg.setStyleSheet("QLabel {color:black; alignment: center;}");
        msg.exec();
    }

}

//一般函数
void MainWindow::parseWeatherJson(QByteArray data) {
// void MainWindow::parseWeatherJson(QString dataStr) {
    // QByteArray data = dataStr.toUtf8();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (!jsonDoc.isNull() && jsonDoc.isObject()) {
        QJsonObject json = jsonDoc.object();
        // qDebug() << "json格式：\n"<<json;
        if (data.contains("data") && json["data"].isArray()) {
            QJsonArray dataArray = json["data"].toArray();
            for (int i = 0; i < dataArray.size(); i++) {
                QJsonObject element = dataArray[i].toObject();
                QString date = element["date"].toString();
                QString week = element["week"].toString();
                QString city = json["city"].toString();
                QString temp = element["tem"].toString();
                QString tempMax = element["tem1"].toString();
                QString tempMin = element["tem2"].toString();
                QString weatherType = element["wea"].toString();
                QString airTip = element["air_tips"].toString();
                QString windDir = element["win"].toArray()[0].toString();
                QString windPower = element["win_speed"].toString();
                QString pm2_5 = json["aqi"].toObject()["pm25"].toString();
                QString humidity =  element["humidity"].toString();
                QString airQuality = element["air_level"].toString();
                if (i==0) {
                    //解析日期
                    ui->dateLabel->setText(date);
                    ui->weekLabel->setText(week);
                    //解析城市名称
                    ui->cityLabel->setText(city);
                    //解析温度
                    ui->degreeLabel->setText(temp+"°C");
                    ui->weatherdegreeLabel->setText(tempMin+"~"+tempMax+"°C");
                    //天气类型
                    ui->weatherTextLabel->setText(weatherType);
                    qDebug() << city << ":" << weatherType;
                    // if (weatherIconMap.find(weatherType) == weatherIconMap.end()) {
                    //     ui->weatherLabel->setPixmap(QPixmap(":weather/images/cloud1.png"));
                    // } else {
                    //     ui->weatherLabel->setPixmap(QPixmap(weatherIconMap[weatherType]).scaled(ui->weatherLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    // }
                    if (weatherIconMap.find(weatherType) == weatherIconMap.end()) {
                        // ui->weatherLabel->setPixmap(QPixmap(":weather/images/cloud1.png"));
                        if (weatherType.contains("转")) {
                            int index = weatherType.indexOf("转");
                            if (weatherIconMap.find(weatherType.left(index)) == weatherIconMap.end()) {
                                ui->weatherLabel->setPixmap(QPixmap(":weather/images/cloud1.png").scaled(ui->weatherLabel->size()*0.5, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                            }else {
                                ui->weatherLabel->setPixmap(QPixmap(weatherIconMap[weatherType.left(index)]).scaled(ui->weatherLabel->size()*0.5, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                            }
                        }else {
                            ui->weatherLabel->setPixmap(QPixmap(":weather/images/cloud1.png").scaled(ui->weatherLabel->size()*0.5, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        }
                    } else {
                        // ui->weatherLabel->setPixmap(QPixmap(weatherIconMap[weatherType]));
                        ui->weatherLabel->setPixmap(QPixmap(weatherIconMap[weatherType]).scaled(ui->weatherLabel->size()*0.5, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                    //感冒指数
                    ui->widet3Label->setText(QString("感冒指数：")+airTip);
                    //风向
                    ui->windDirectLabel->setText(windDir);
                    //风力
                    ui->windLevelLabel->setText(windPower);
                    //pm2.5
                    ui->pmLevelLabel->setText(pm2_5);
                    //humid
                    ui->humidLevelLabel->setText(humidity);
                    //空气质量
                    ui->airLevelLabel->setText(airQuality);
                }
                days[i].mAirq = airQuality;
                days[i].mCity = city;
                days[i].mDate = date;
                days[i].mWeek = week;
                days[i].mFl = windPower;
                days[i].mFx = windDir;
                days[i].mHumidity = humidity;
                days[i].mPm2_5 = pm2_5;
                days[i].mTemp = temp;
                days[i].mTempHigh = tempMax;
                days[i].mTempLow = tempMin;
                days[i].mTips = airTip;
                days[i].mWeatherType = weatherType;
                if (weatherIconMap.find(weatherType) == weatherIconMap.end()) {
                    // ui->weatherLabel->setPixmap(QPixmap(":weather/images/cloud1.png"));
                    if (weatherType.contains("转")) {
                        int index = weatherType.indexOf("转");
                        if (weatherIconMap.find(weatherType.left(index)) == weatherIconMap.end()) {
                            days[i].mFileName = QString(":weather/images/cloud1.png");
                        }else {
                            days[i].mFileName = weatherIconMap[weatherType.left(index)];
                        }
                    }else {
                        days[i].mFileName = QString(":weather/images/cloud1.png");
                    }
                } else {
                    // ui->weatherLabel->setPixmap(QPixmap(weatherIconMap[weatherType]));
                    days[i].mFileName = weatherIconMap[weatherType];
                }
            }
        }

    }
}


QString getCityCodeFromJson(QString cityName) {
    QFile file("/Users/liuchuanxi/项目/QT/Qt-project/天气预报/weather/cityCode.json");
    bool fileFlag = file.open(QIODevice::ReadOnly|QIODevice::Text);
    if (!fileFlag) {
        QMessageBox msg;
        msg.setWindowTitle("城市数据文件错误");
        msg.setText("打开文件失败");
        msg.setIconPixmap(QPixmap(":weather/images/alert.png").scaled(20,20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        msg.setStyleSheet("QPushButton {color:white; display:grid; justify-content: center; place-items: center;}");
        msg.setStyleSheet("QLabel {color:black; alignment: center;}");
        msg.exec();
        return QString();
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!jsonDoc.isNull() && jsonDoc.isArray()) {
        QJsonArray provinces = jsonDoc.array();
        for (QJsonValue province: provinces) {
            if (province.isObject()) {
                QString provinceNameLocal = province["name"].toString();
                if (provinceNameLocal == cityName) {
                    return province["code"].toString();
                }
            }
            QJsonArray cities = province["city"].toArray();
            for (QJsonValue city: cities) {
                if (city.isObject()) {
                    QString cityNameLocal = city["name"].toString();
                    if (cityNameLocal == cityName) {
                        return city["code"].toString();
                    }
                }
            }
        }
    }
    return QString();
}


void MainWindow::on_searchLabel_clicked() {
    QString cityNameFromUser = ui->searchLineEdit->text();
    QString cityCode;
    QString enterShortcut = QString();
    if (cityNameFromUser.contains("市")) {
        if (cityNameFromUser.contains("\n")) {
            enterShortcut = "\n";
            cityNameFromUser.chop(1);
            cityCode = cityCodeUtils.getCityCodeFromName(cityNameFromUser);/*getCityCodeFromJson(cityNameFromUser);*/
        }else {
            cityCode = cityCodeUtils.getCityCodeFromName(cityNameFromUser);/*getCityCodeFromJson(cityNameFromUser);*/
        }
    }else {
        if (cityNameFromUser.contains("\n")) {
            enterShortcut = "\n";
            cityNameFromUser.chop(1);
            cityCode =cityCodeUtils.getCityCodeFromName(cityNameFromUser+"市"); /*getCityCodeFromJson(cityNameFromUser+"市");*/
        }else {
            cityCode = cityCodeUtils.getCityCodeFromName(cityNameFromUser+"市");/*getCityCodeFromJson(cityNameFromUser+"市");*/
        }
    }
    QString urlRootStr = urlStr;
    if (cityCode != NULL) {
        urlStr = urlStr + "&cityid=" + cityCode;
        QUrl urlUrl(urlStr);
        QNetworkRequest request(urlUrl);
        request.setRawHeader("Content-Type", "application/json; charset=UTF-8");
        // QNetworkReply* reply = manager->get(request);
        manager->get(request);
        ui->cityLabel->setText(cityNameFromUser);
    }else {
        QMessageBox msg;
        msg.setWindowTitle("城市搜索");
        msg.setText(QString("%1不在搜索列表中").arg(cityNameFromUser));
        msg.setIconPixmap(QPixmap(":weather/images/alert.png").scaled(20,20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        msg.setStyleSheet("QPushButton {color:white; display:grid; justify-content: center; place-items: center;}");
        msg.setStyleSheet("QLabel {color:black; alignment: center;}");
        msg.exec();
    }
    //还原URL为非城市的URL
    urlStr = urlRootStr;
}

void MainWindow::updateUIFace() {
    for (int i=0; i<mDateList.size(); i++) {
        QPixmap pic;
        QString dateFormat = days[i].mDate;
        QStringList dateList = dateFormat.split("-");
        mDateList[i]->setText(dateList[1]+"/"+dateList[2]);
        mWeekList[i]->setText(days[i].mWeek);
        mAirqList[i]->setText(days[i].mAirq);
        if (days[i].mAirq == "优") {
            mAirqList[i]->setStyleSheet("QLabel {background-color: rgb(84, 175, 24); border-radius: 5px; color:rgb(255,255,255);}");
        }else if (days[i].mAirq == "良") {
            mAirqList[i]->setStyleSheet("QLabel {background-color: rgb(219, 222, 35); border-radius: 5px; color: rgb(255,255,255);}");
        } else {
            mAirqList[i]->setStyleSheet("QLabel {background-color: rgb(255, 97, 30); border-radius: 5px; color: rgb(255,255,255);}");
        }
        mFxList[i]->setText(days[i].mFx);
        mFlList[i]->setText(days[i].mFl);
        mWeatherTypList[i]->setText(days[i].mWeatherType);
        pic = QPixmap(days[i].mFileName);
        pic = pic.scaled(mIconList[i]->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        mIconList[i]->setPixmap(QPixmap(days[i].mFileName));
    }

}

void MainWindow::drawTempLine(Day* days, QString key, QWidget* parent, Style styleMap) { //
    QPainter painter(parent);
    painter.setPen(styleMap.color);
    painter.setBrush(styleMap.brush);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QList<QPointF> points;//[mAirqList.size()];

    //计算均值
    double meanTemp;
    double sumTemp = 0.;
    for (int i=0; i<mAirqList.size(); i++) {
        QString temp;
        if (key == "mTempHigh") {
            temp= days[i].mTempHigh;
        } else if (key == "mTempLow") {
            temp= days[i].mTempLow;
        }
        double tempD = temp.toDouble();
        // if (i==0) {
        //     minTemp = tempD;
        //     maxTemp = tempD;
        // }else {
        //     if (tempD < minTemp) minTemp = tempD;
        //     if (tempD > maxTemp) maxTemp = tempD;
        // }
        sumTemp += tempD;
    }
    meanTemp = sumTemp / mAirqList.size();
    //计算偏差
    QList<double> offset;
    for (int i=0; i<mAirqList.size(); i++) {
        QString temp;
        if (key == "mTempHigh") {
            temp= days[i].mTempHigh;
        } else if (key == "mTempLow") {
            temp= days[i].mTempLow;
        }
        double tempD = temp.toDouble();
        offset << tempD - meanTemp;
    }
    //归一化偏差
    double maxTemp;
    double minTemp;
    QList<double> offsetNorm;
    for (int i=0; i<mAirqList.size(); i++) {
        double tempD = offset[i];
        if (i==0) {
            minTemp = tempD;
            maxTemp = tempD;
        }else {
            if (tempD < minTemp) minTemp = tempD;
            if (tempD > maxTemp) maxTemp = tempD;
        }
    }
    for (int i=0; i<mAirqList.size(); i++) {
        if (offset[i] <= 0) {
            offsetNorm << offset[i]/qAbs<double>(minTemp);
        } else {
            offsetNorm << offset[i]/qAbs<double>(maxTemp);
        }
    }

    // double height = parent->height()*0.9;
    double halfHeight = parent->height()*styleMap.frameScaled/2; // height/2;
    //因为画笔是放在自控件上作画而不是整个控件上作画所以不需要自控件在父控件中的坐标位置
    // double posCenY = parent->pos().y()+parent->height()*0.5;
    double posCenY = parent->height()*0.5 + ( parent->height() - styleMap.marginBottom - ( parent->height()*0.5 + halfHeight ) );
    for (int i=0; i<mAirqList.size(); i++) {
        QPointF curPos;
        curPos.setX(mAirqList[i]->pos().x()+mAirqList[i]->width()/2);
        if (offsetNorm[i] <= 0) {
            curPos.setY(posCenY + halfHeight*qAbs<double>(offsetNorm[i]));
        } else {
            curPos.setY(posCenY - halfHeight*qAbs<double>(offsetNorm[i]));
        }
        points << curPos;
        //画点
        painter.drawEllipse(points[i], 2, 2);
        //画线
        if (i > 0) {
            painter.drawLine(points[i], points[i-1]);
        }
        //写字
        QString temp;
        if (key == "mTempHigh") {
            temp= days[i].mTempHigh;
        } else if (key == "mTempLow") {
            temp= days[i].mTempLow;
        }
        QPointF textPos;
        textPos.setX(points[i].x()-15);
        textPos.setY(points[i].y()-12);
        painter.drawText(textPos, temp+"℃");
    }
}

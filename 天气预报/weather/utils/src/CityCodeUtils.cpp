//
// Created by chauchy on 25-6-26.
//

#include "../include/mapUtil/CityCodeUtils.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>

CityCodeUtils::CityCodeUtils() {
}

QString CityCodeUtils::getCityCodeFromName(QString cityName) {
    if (cityMap.isEmpty()) {
        initCityMap();
    }
    // QMap<QString, QString>::iterator cityIter = cityMap.find(cityName);
    // if (cityIter == cityMap.end()) {
    //     return QString();
    // } else {
    //     return cityIter.value();
    // }
    for (QMap<QString, QString>::iterator cityIter = cityMap.begin(); cityIter != cityMap.end(); cityIter++) {
        if (cityIter.key().contains(cityName) || cityName.contains(cityIter.key())) {
            return cityIter.value();
        }
    }
    return QString();
}

void CityCodeUtils::initCityMap() {
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
        // return QString();
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!jsonDoc.isNull() && jsonDoc.isArray()) {
        QJsonArray provinces = jsonDoc.array();
        for (QJsonValue province: provinces) {
            QString provinceNameLocal;
            QString provinceCode;
            if (province.isObject()) {
                provinceNameLocal = province["city_name"].toString();
                provinceCode = province["city_code"].toString();
                cityMap[provinceNameLocal] = provinceCode;
                // if (provinceNameLocal == cityName) {
                //     // return province["code"].toString();
                // }
            }
            /*QJsonArray cities = province["city"].toArray();
            for (QJsonValue city: cities) {
                if (city.isObject()) {
                    QString cityNameLocal = city["name"].toString();
                    QString cityCodeLocal = city["code"].toString();
                    if (cityNameLocal == "市辖区") {
                        cityMap[provinceNameLocal] = provinceCode;
                    } else {
                        cityMap[cityNameLocal] = cityCodeLocal;
                    }
                    // if (cityNameLocal == cityName) {
                    //     // return city["code"].toString();
                    // }
                }
            }*/
        }
    }
    // return QString();
}

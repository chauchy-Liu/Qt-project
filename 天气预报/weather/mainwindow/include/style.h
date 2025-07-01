//
// Created by chauchy on 25-7-1.
//
#include <QColor>

#ifndef STYLE_H
#define STYLE_H
class Style {
public:
    QColor color;
    QBrush brush;
    Qt::PenStyle lineStyle;
    double frameOffsetY;
    double frameScaled;
    double marginBottom;
};
#endif //STYLE_H

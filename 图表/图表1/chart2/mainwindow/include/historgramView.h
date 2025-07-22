//
// Created by chauchy on 25-7-2.
//

#ifndef HISTORGRAMVIEW_H
#define HISTORGRAMVIEW_H
#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QRegion>
#include <QMouseEvent>
#include <QList>

class HistorgramView: public QAbstractItemView {
    Q_OBJECT
public:
    HistorgramView(QWidget *parent = 0);
    ~HistorgramView();
    QRect visualRect(const QModelIndex &index) const;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
    QModelIndex indexAt(const QPoint &point) const;
    void setSelectionModel(QItemSelectionModel *selectionModel);
    QRegion itemRegion(const QModelIndex &index) const;
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private:
    QItemSelectionModel* selections;
    QList<QRegion> mRegionList;
    QList<QRegion> fRegionList;
    QList<QRegion> tRegionList;


};



#endif //HISTORGRAMVIEW_H

//
// Created by chauchy on 25-5-21.
//

#ifndef SOCKETCOMBOBOX_H
#define SOCKETCOMBOBOX_H

#include <QComboBox>
#include <QWidget>

class SocketComboBox : public QComboBox {
    Q_OBJECT
public:
    SocketComboBox(QWidget* parent);

protected:
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void on_ComboBox_clicked();

};

#endif //SOCKETCOMBOBOX_H

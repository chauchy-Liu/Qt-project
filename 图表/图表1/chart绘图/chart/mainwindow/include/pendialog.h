//
// Created by chauchy on 25-7-8.
//

#ifndef PENDIALOG_H
#define PENDIALOG_H

#include <QDialog>
#include <QPen>


QT_BEGIN_NAMESPACE
namespace Ui { class PenDialog; }
QT_END_NAMESPACE

class PenDialog : public QDialog {
Q_OBJECT
public:
    static QPen getPen(QPen iniPen, bool& ok);
public:
    explicit PenDialog(QWidget *parent = nullptr);
    ~PenDialog() override;
    QPen getPen();
    void setPen(QPen &pen);

private slots:
void on_yesBtn_clicked();

void on_noBtn_clicked();

void on_colorBtn_clicked();

private:
    Ui::PenDialog *ui;
    QPen mPen;

};


#endif //PENDIALOG_H

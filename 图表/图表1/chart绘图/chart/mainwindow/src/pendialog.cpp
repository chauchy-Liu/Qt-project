//
// Created by chauchy on 25-7-8.
//

// You may need to build the project (run Qt uic code generator) to get "ui_penDialog.h" resolved

#include "pendialog.h"

#include <qcolordialog.h>

#include "ui_penDialog.h"
#include <QDebug>

/**
 * @brief
 * @param iniPen
 * @param ok
 * @return
 */
QPen PenDialog::getPen(QPen iniPen, bool &ok) {
    //静态函数作用域中没有对象this, 创建对象
    PenDialog* pDlg = new PenDialog;
    pDlg->setPen(iniPen);
    QPen pen;
    //生成对话框
    int ret = pDlg->exec();
    if (ret == QDialog::Accepted) {
        pen = pDlg->getPen();
        ok = true;
    } else {
        pen = iniPen;
        ok = false;
    }
    delete pDlg;
    return pen;
}

PenDialog::PenDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::PenDialog) {
    ui->setupUi(this);
    ui->styleComboBox->clear();
    ui->styleComboBox->addItem(tr("NoPen"), 0);
    ui->styleComboBox->addItem(tr("SolidLine"), 1);
    ui->styleComboBox->addItem(tr("DashLine"), 2);
    ui->styleComboBox->addItem(tr("DotLine"), 3);
    ui->styleComboBox->addItem(tr("DashDotLine"), 4);
    ui->styleComboBox->addItem(tr("DashDotDotLine"), 5);
    ui->styleComboBox->addItem(tr("CustomDashLine"), 6);
    ui->styleComboBox->setCurrentIndex(1);
}

PenDialog::~PenDialog() {
    delete ui;
}

/**
 * @brief 从窗口输入的内容录入到mPen成员中
 * @return  mPen
 */
QPen PenDialog::getPen() {
    mPen.setStyle(Qt::PenStyle(ui->styleComboBox->currentIndex()));
    mPen.setWidth(ui->widthSpinBox->value());
    QColor color = ui->colorBtn->palette().color(QPalette::Button);
    mPen.setColor(color);
    return mPen;
}

/**
 * @brief 从外部Pen变量中设置mPen和窗口内容
 * @param pen
 */
void PenDialog::setPen(QPen& pen) {
    mPen = pen;
    ui->widthSpinBox->setValue(pen.width());
    //将枚举量转成对应的数值
    int i = static_cast<int>(pen.style());
    ui->styleComboBox->setCurrentIndex(i);
    ui->colorBtn->setAutoFillBackground(true);
    QColor color = pen.color();
    // qDebug() << QString("QPushButton {background-color:rgb(%1, %2, %3);}").arg(color.red()).arg(color.green()).arg(color.blue());
    ui->colorBtn->setStyleSheet(QString("QPushButton {background-color:rgb(%1, %2, %3);}").arg(color.red()).arg(color.green()).arg(color.blue()));
}

void PenDialog::on_yesBtn_clicked() {
    this->accept();
}

void PenDialog::on_noBtn_clicked() {
    this->reject();
}

void PenDialog::on_colorBtn_clicked() {
    QColor color = mPen.color();
    color = QColorDialog::getColor(color);
    if (color.isValid()) {
        ui->colorBtn->setStyleSheet(QString("QPushButton {background-color:rgb(%1, %2, %3);}").arg(color.red()).arg(color.green()).arg(color.blue()));
    }

}



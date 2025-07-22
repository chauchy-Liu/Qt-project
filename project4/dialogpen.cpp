#include "dialogpen.h"
#include "ui_dialogpen.h"

dialogPen::dialogPen(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialogPen)
{
    ui->setupUi(this);
}

dialogPen::~dialogPen()
{
    delete ui;
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MyButton* btn = new MyButton(this);
    btn->move(100,100);
    btn->setFixedSize(200,50);
    //使用在MyButton中自定义的信号与槽
    connect(btn, &MyButton::isClicked, this, [=]() {
        qDebug() << "btn clicked";
    });
    connect(btn, &MyButton::isClickedWithParam, this, [=](bool value) {
        qDebug() << "btn clicked" << value;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //虽然上行代码将自己的Widget和Ui::Widget进行关联，但是如果发生窗体大小变化的时候，里面的布局不会随之变化
    //通过下面这行代码显示说明，让窗口变化时，布局及其子控件随之调整
    ui->centralwidget->setLayout(ui->verticalLayout);
    ui->statWidget->setLayout(ui->bottomHLayout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

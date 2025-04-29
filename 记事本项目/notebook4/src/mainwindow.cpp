#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //虽然上行代码将自己的Widget和Ui::Widget进行关联，但是如果发生窗体大小变化的时候，里面的布局不会随之变化
    //通过下面这行代码显示说明，让窗口变化时，布局及其子控件随之调整
    ui->centralwidget->setLayout(ui->verticalLayout);
    ui->statWidget->setLayout(ui->bottomHLayout);
    //在构造函数中进行信号与槽的绑定
    //第一种on_objectName_signalName方式不需要在构造函数中连接信号与槽
    //第二种方式QObject::connect(sender, SIGNAL(signal), receiver, SLOT(slot));
    QObject::connect(ui->btnOldConnect, SIGNAL(clicked()), this, SLOT(btnOldConnectFunc()));
    //第三种方式：Lambda表达式QObject::connect(sender, &SenderClass::signal, [=](){/* lambda body */})
    QObject::connect(ui->btnLambda, &QPushButton::clicked, [=](){
        std::cout << "Lambda Button Clicked: QObject::connect(sender, &SenderClass::signal, [=](){/* lambda body */})" << std::endl;
    });
    //第四种方式:函数数指针QObject::connect(sender, &Sender::signal, receiver, &Receiver::slot);
    QObject::connect(ui->btnNewConnect, &QPushButton::clicked, this, &MainWindow::btnNewConnectFunc);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//第一种连接信号与槽方式
void MainWindow::on_btnCloseIcon_clicked()
{
    std::cout << "Close Icon Clicked: on_objectName_signalName format" << std::endl;
    this->close();
}

void MainWindow::btnOldConnectFunc()
{
    std::cout << "Old Connect Button Clicked: QObject::connect(sender, SIGNAL(signal), receiver, SLOT(slot));" << std::endl;
}

void MainWindow::btnNewConnectFunc()
{
    std::cout << "New Connect Button Clicked: QObject::connect(sender, &Sender::signal, receiver, &Receiver::slot);" << std::endl;
}
//读文件
void MainWindow::on_btnOpenIcon_clicked(){
    //1. 打开文件
    QFile file("./test.txt");// QFile file; file.setFileName("./test.txt");
    bool open_flag = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!open_flag){
        qDebug() << "Open File Failed!";
        return;
    }
    int size = file.size();
    //2. 读取文件
    // char context[100] = {'\0'};
    char* context = new char[size];
    qint64 read_flag = file.read(context, size);
    if (read_flag == -1){
        qDebug() << "Read File Failed!";
        return ;
    }
    //3. 输出文件内容
    qDebug() << "File Content: " << context;
    //4. 关闭文件
    file.close();
    delete[] context;
    context = nullptr;
}
//保存文件
void MainWindow::on_btnSaveIcon_clicked(){
    //1. 打开文件
    QFile file = QFile("./test1.txt");
    bool open_flag = file.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!open_flag){
        qDebug() << "Open File Failed!";
        return;
    }
    //2. 写入文件
    qint64 write_flag = file.write("Program 45-QFile001 write something to this File, 我是传玺");
    if(write_flag==-1){
        qDebug() << "Write File Failed!";
        return;
    }
    //3. 关闭文件
    file.close();
}
//QTextCodec
void MainWindow::on_btnIn_clicked(){
    QFile file("./test.txt");
    bool open_flag = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!open_flag){
        qDebug() << "Open File Failed!";
        return;
    }
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);//("UTF-8");
    int context = file.size();
    qDebug() << "File size: " << context;
    while(!in.atEnd()){
        QString line = in.readLine();
        qDebug() << "File Content: " << line;
    }
}

void MainWindow::on_btnOut_clicked(){
    QFile file("./test1.txt");
    bool open_flag = file.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!open_flag){
        qDebug() << "Open File Failed!";
        return;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "Program 45-QFile001 write something to this File, 我是传玺, 文本字符流";
    file.close();
}
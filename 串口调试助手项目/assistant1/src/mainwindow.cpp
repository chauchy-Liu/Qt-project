#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //窗口内容随窗口大小变化
    ui->centralwidget->setLayout(ui->windowGridLayout);//ui->centralwidget
    //自动检测可用串口，并添加到comboBox中
    QList<QSerialPortInfo> serialList = QSerialPortInfo::availablePorts();
    for (QSerialPortInfo serial : serialList) {
        qDebug() << serial.portName();
        ui->chuanComboBox->addItem(serial.portName());
    }
    //设置comboBox的默认值
    ui->boTeComboBox->setCurrentIndex(6);
    ui->dataComboBox->setCurrentIndex(3);
    //设置串口对象
    serialPort = new QSerialPort(this);
    this->writeCount = 0;
    this->readCount = 0;
    isSerialPortOpen = false;
    //设置pushbutton不可用
    ui->sendPushButton->setEnabled(false);
    //给窗体对象安装定时器
    timer = new QTimer(this);
    //设置信号与槽
    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::onSerialDataReadyToRead);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_sendPushButton_clicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_closePushButton_clicked()
{
    qDebug() << "串口打开关闭槽函数";
    //1.选择端口号
    serialPort->setPortName(ui->chuanComboBox->currentText());
    //2.配置波特率
    serialPort->setBaudRate(ui->boTeComboBox->currentText().toInt());
    //3.配置数据位
    serialPort->setDataBits(QSerialPort::DataBits(ui->dataComboBox->currentText().toUInt()));
    //4.配置校验位
    switch (ui->validComboBox->currentIndex()) {
        case 0:
            serialPort->setParity(QSerialPort::NoParity);
            break;
        case 2:
            serialPort->setParity(QSerialPort::MarkParity);
            break;
        case 1:
            serialPort->setParity(QSerialPort::EvenParity);
            break;
        case 3:
            serialPort->setParity(QSerialPort::OddParity);
            break;
        case 4:
            serialPort->setParity(QSerialPort::SpaceParity);
            break;
    }
    //5.配置停止位
    // serialPort->setStopBits(QSerialPort::StopBits(ui->stopComboBox->currentText().toUInt()));
    switch (ui->stopComboBox->currentIndex()) {
        case 0:
            serialPort->setStopBits(QSerialPort::OneStop);
            break;
        case 1:
            serialPort->setStopBits(QSerialPort::OneAndHalfStop);
            break;
        case 2:
            serialPort->setStopBits(QSerialPort::TwoStop);
            break;
        default: 
            break;
    }
    //6.配置流控
    if (ui->streamComboBox->currentText() == "No") {
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
    }

    //7.打开串口
    if (!isSerialPortOpen){
        if (serialPort->open(QIODevice::ReadWrite)) {
            qDebug() << "串口打开成功";
            //设置combox为无法选择状态
            ui->chuanComboBox->setEnabled(false);
            ui->boTeComboBox->setEnabled(false);
            ui->dataComboBox->setEnabled(false);
            ui->validComboBox->setEnabled(false);
            ui->stopComboBox->setEnabled(false);
            ui->streamComboBox->setEnabled(false);
            //设置pushbutton的文字为关闭
            ui->closePushButton->setText("关闭串口");
            //设置pushbutton可用
            ui->sendPushButton->setEnabled(true);
            ui->timeSendCheckBox->setEnabled(true);
            ui->sendLineEdit->setEnabled(true);
            ui->freqLineEdit->setEnabled(true);
            ui->newLineSendCheckBox->setEnabled(true);
            ui->HexSendCheckBox->setEnabled(true);
            ui->formatInputCheckBox->setEnabled(true);
            //设置串口打开状态
            isSerialPortOpen = true;
        } else {
            qDebug() << "串口打开失败";
            QMessageBox msgBox;
            msgBox.setWindowTitle("打开串口错误");
            msgBox.setIconPixmap(QPixmap(":icon/res/noun-serial-port-5265721.png"));
            msgBox.setText("串口可能被占用");
            msgBox.exec();
        }
    }else{
        qDebug() << "串口关闭成功";
        serialPort->close();
        //设置combox为可以选择状态
        ui->chuanComboBox->setEnabled(true);
        ui->boTeComboBox->setEnabled(true);
        ui->dataComboBox->setEnabled(true);
        ui->validComboBox->setEnabled(true);
        ui->stopComboBox->setEnabled(true);
        ui->streamComboBox->setEnabled(true);
        //设置pushbutton的文字为打开
        ui->closePushButton->setText("打开串口");
        //设置串口关闭状态
        isSerialPortOpen = false;
        //设置pushbutton不可用
        ui->sendPushButton->setEnabled(false);
        ui->timeSendCheckBox->setEnabled(false);
        ui->sendLineEdit->setEnabled(false);
        ui->freqLineEdit->setEnabled(false);
        ui->newLineSendCheckBox->setEnabled(false);
        ui->HexSendCheckBox->setEnabled(false);
        ui->formatInputCheckBox->setEnabled(false);
        //定时勾选取消
        ui->timeSendCheckBox->setCheckState(Qt::Unchecked);
        //定时器关闭
        timer->stop();
    }
    
}

void MainWindow::on_sendPushButton_clicked(){
    int writeCount = 0;
    qDebug() << "发送数据槽函数";
    //获取发送数据
    qDebug() << "ui框中数据:" << ui->sendLineEdit->text();
    QString sendData = ui->sendLineEdit->text();
    qDebug() << "串口对象写入的数据:" << sendData.toUtf8();
    //发送数据
    writeCount = serialPort->write(sendData.toUtf8());

    if (writeCount == -1){
        //状态栏
        QFont font = ui->statusLabel->font();
        font.setBold(true);
        ui->statusLabel->setFont(font);
        ui->statusLabel->setText("Send Failed!");
    }else{
        //状态栏
        this->writeCount += writeCount;
        QFont font = ui->statusLabel->font();
        font.setBold(true);
        ui->statusLabel->setFont(font);
        ui->statusLabel->setText("Send OK!");
        
        ui->sentLabel->setText(QString("Sent: %1").arg(this->writeCount)); // setNum(this->writeCount);
    }

    //***************************************************** */
    //模拟串口自发自收
    qDebug() << "接收数据槽函数";
    //读取数据
    // QByteArray data = ui->sendLineEdit->text(); //serialPort->readAll();
    // qDebug() << "串口对象读取的Byte数据:" << data;
    // QString revMessage = QString::fromUtf8(data);
    // qDebug() << "串口对象读取的string数据:" << revMessage;
    QString revMessage = ui->sendLineEdit->text();
    qDebug() << "串口对象读取的string数据:" << revMessage;
    //显示在ui框中
    ui->textEdit->append(revMessage);
    //****************************************************** */
    //状态栏
    if (writeCount != -1){
        this->readCount += revMessage.toUtf8().size();
        qDebug() << "串口对象读取的字节数据:" << revMessage.toUtf8();
        ui->RevLabel->setText(QString("Received: %1").arg(this->readCount)); //setNum(this->readCount);
        //显示在ui框中
        if (this->sendHis != revMessage){
            this->sendHis = revMessage;
            ui->textEditHis->append(revMessage);
        }
    }


}

void MainWindow::onSerialDataReadyToRead(){
    qDebug() << "接收数据槽函数";
    //读取数据
    QByteArray data = serialPort->readAll();
    if (!data.isEmpty()){
        qDebug() << "串口对象读取的Byte数据:" << data;
        QString revMessage = QString::fromUtf8(data);
        qDebug() << "串口对象读取的string数据:" << revMessage;
        //显示在ui框中
        ui->textEdit->append(revMessage);
        this->readCount += data.size();
        ui->RevLabel->setText(QString("Received: %1").arg(this->readCount)); //setNum(this->readCount);
    }
    
}

void MainWindow::on_timeSendCheckBox_clicked(bool checked)
{
    qDebug() << "定时发送数据槽函数, 是否勾选：" << checked;
    if (checked) {
        on_sendPushButton_clicked();
        int sendDuration = ui->freqLineEdit->text().toInt();
        timer->start(sendDuration);
        ui->freqLineEdit->setEnabled(false);
        ui->sendPushButton->setEnabled(false);
        ui->sendLineEdit->setEnabled(false);
    }else{
        timer->stop();
        ui->freqLineEdit->setEnabled(true);
        ui->sendPushButton->setEnabled(true);
        ui->sendLineEdit->setEnabled(true);
    }

}
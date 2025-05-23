#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QFileDialog>

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
    //设置pushbutton不可用
    ui->sendPushButton->setEnabled(false);
    //给窗体对象安装定时器
    timer = new QTimer(this);
    //设置时间状态栏定时器
    QTimer* statusTimer = new QTimer(this);
    statusTimer->start(60000);

    //设置初始化成员变量
    isSerialPortOpen = false;
    isTimeBoxChecked = false;

    //设置信号与槽
    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::onSerialDataReadyToRead);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_sendPushButton_clicked);
    connect(statusTimer, &QTimer::timeout, this, &MainWindow::refreshStatusTime);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//########################槽函数#################################
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
    if (ui->HexSendCheckBox->isChecked()){
        QByteArray sendDataHex = sendData.toUtf8();
        //检测输入Hex是否符合Hex的位数为偶数
        if (sendData.size() % 2 != 0){
            //状态栏
            QFont font = ui->statusLabel->font();
            font.setBold(true);
            ui->statusLabel->setFont(font);
            ui->statusLabel->setText("Hex Odd!");
            return;
        }
        //检测输入Hex是否符合Hex的取值范围
        for (QChar a: sendData){
            if (!(a.isDigit() || (a >= 'A' && a <= 'F') || (a >= 'a' && a <= 'f'))){
                //不合法
                //状态栏
                QFont font = ui->statusLabel->font();
                font.setBold(true);
                ui->statusLabel->setFont(font);
                ui->statusLabel->setText("Hex Out of Range!");
                return;
            }
        }
        //将Hex编码转成ASSII编码
        writeCount = serialPort->write(QByteArray::fromHex(sendDataHex));
    }else{
        writeCount = serialPort->write(sendData.toUtf8());
    }

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
    //在HexBox选中状态下输入ASSII格式转成Hex格式
    if (ui->hexShowCheckBox->checkState() == Qt::Checked && ui->HexSendCheckBox->isChecked() == false){
        //添加换行符
        if (ui->nextLineCheckBox->isChecked()){
            revMessage = "\n" + revMessage;
        }
        if (ui->newLineSendCheckBox->isChecked()){
            revMessage = revMessage + "\n";
        }
        //ASSIIQString转QByteArray
        QByteArray tmpByteArray = revMessage.toUtf8();
        //ASSIIQByteArray转成16进制QByteArray
        QByteArray tmpHex = tmpByteArray.toHex();
        //16进制QByteArray转成QString
        revMessage = QString::fromUtf8(tmpHex).toUpper();
    } else if (ui->hexShowCheckBox->isChecked() == false && ui->HexSendCheckBox->isChecked() == false){
        //添加换行符
        if (ui->nextLineCheckBox->isChecked()){
            revMessage = "\n" + revMessage;
        }
        if (ui->newLineSendCheckBox->isChecked()){
            revMessage = revMessage + "\n";
        }
    } else if (ui->hexShowCheckBox->isChecked() == true && ui->HexSendCheckBox->isChecked() == true){
        //添加换行符
        QString newLine = "\n";
        //ASSIIQString转QByteArray
        QByteArray tmpByteArray = newLine.toUtf8();
        //ASSIIQByteArray转成16进制QByteArray
        QByteArray tmpHex = tmpByteArray.toHex();
        if (ui->nextLineCheckBox->isChecked()){
            //16进制QByteArray转成QString
            revMessage = QString::fromUtf8(tmpHex) + revMessage;
        }
        if (ui->newLineSendCheckBox->isChecked()){
            revMessage = revMessage + QString::fromUtf8(tmpHex);
        }
        revMessage = revMessage.toUpper();
    } else if (ui->hexShowCheckBox->isChecked() == false && ui->HexSendCheckBox->isChecked() == true){
        //添加换行符
        QString newLine = "\n";
        //ASSIIQString转QByteArray
        QByteArray tmpByteArray = newLine.toUtf8();
        //ASSIIQByteArray转成16进制QByteArray
        QByteArray tmpHex = tmpByteArray.toHex();
        if (ui->nextLineCheckBox->isChecked()){
            //16进制QByteArray转成QString
            revMessage = QString::fromUtf8(tmpHex) + revMessage;
        }
        if (ui->newLineSendCheckBox->isChecked()){
            revMessage = revMessage + QString::fromUtf8(tmpHex);
        }
        //将Hex解码转成ASSII解码
        revMessage = QString::fromUtf8(QByteArray::fromHex(revMessage.toUtf8()));
    }
    //显示在ui框中
    if (isTimeBoxChecked){
        //添加时间戳
        getSystemTime();
        QString revTimeFormatMessage = "【" + myTime + "】 " + revMessage;
        // ui->textEdit->append(revTimeFormatMessage);
        ui->textEdit->insertPlainText(revTimeFormatMessage);
    }else{
        // ui->textEdit->append(revMessage);
        ui->textEdit->insertPlainText(revMessage);
    }

    //****************************************************** */
    //状态栏
    if (writeCount != -1){
        this->readCount += revMessage.toUtf8().size();
        qDebug() << "串口对象读取的字节数据:" << revMessage.toUtf8();
        ui->RevLabel->setText(QString("Received: %1").arg(this->readCount)); //setNum(this->readCount);
        //显示在ui框中
        //在HexBox选中状态下使用输入的ASSII作为revMessage,而非Hex作为revMessage
        if (ui->hexShowCheckBox->checkState() == Qt::Checked && ui->HexSendCheckBox->isChecked() == false){
            revMessage = ui->sendLineEdit->text();
        } else if (ui->hexShowCheckBox->isChecked() == false && ui->HexSendCheckBox->isChecked() == false){

        } else if (ui->hexShowCheckBox->isChecked() == true && ui->HexSendCheckBox->isChecked() == true){
            revMessage = QString::fromUtf8(QByteArray::fromHex(ui->sendLineEdit->text().toUtf8()));
        } else if (ui->hexShowCheckBox->isChecked() == false && ui->HexSendCheckBox->isChecked() == true){
            //将Hex解码转成ASSII解码
            revMessage = QString::fromUtf8(QByteArray::fromHex(ui->sendLineEdit->text().toUtf8()));
        } 
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

void MainWindow::on_clearPushButton_clicked(){
    qDebug() << "清空数据槽函数";
    //清空发送和接收数据
    ui->textEdit->clear();
    //状态栏
    this->writeCount = 0;
    this->readCount = 0;
    ui->sentLabel->setText("Sent: 0");
    ui->RevLabel->setText("Received: 0");
    QFont font = ui->statusLabel->font();
    font.setBold(true);
    ui->statusLabel->setFont(font);
    ui->statusLabel->setText("clear");

}

void MainWindow::on_saveRevPushButton_clicked(){
    qDebug() << "保存接收数据槽函数";
    //获取接收数据
    QString revMessage = ui->textEdit->toPlainText();
    //保存到文件
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "./", tr("Text Files (*.txt);;All Files (*)"));
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file for writing: %1").arg(file.errorString()));
        return;
    }
    QTextStream out(&file);
    out << revMessage;
    file.close();
}

void MainWindow::refreshStatusTime()
{
    getSystemTime();
    //更新状态栏时间
    ui->DateLabel->setText(myTime);
}

void MainWindow::on_revTimeCheckBox_clicked(bool checked)
{
    qDebug() << "接收时间戳槽函数, 是否勾选：" << ui->revTimeCheckBox->isChecked();
    if (ui->revTimeCheckBox->isChecked()) {
        isTimeBoxChecked = true;   
    }else{
        isTimeBoxChecked = false;   
    }
}

void MainWindow::on_hexShowCheckBox_clicked(bool checked)
{
    if (checked){
        //1.读取textEdit中的数据
        QString tmp = ui->textEdit->toPlainText();
        qDebug() << "读取的文本数据String类型:" << tmp;
        //QString转QByteArray（utf-8编码）
        QByteArray tmpByteArray = tmp.toUtf8();
        qDebug() << "转换前的文本数据Byte类型:" << tmpByteArray;
        //2.转换成16进制
        //ASSIIQByteArray转成16进制QByteArray
        QByteArray tmpHex = tmpByteArray.toHex();
        qDebug() << "转换后的Hex数据Byte类型:" << tmpHex;
        //3.显示在textEdit中
        //16进制QByteArray转成QString
        QString tmpHexString1= QString::fromUtf8(tmpHex).toUpper();
        QString tmpHexString;
        qDebug() << "转换后的Hex数据String类型:" << tmpHexString;
        for (int i=0; i<tmpHexString1.size(); i+=2){
            tmpHexString += tmpHexString1.mid(i, 2) + " ";
        }
        ui->textEdit->setText(tmpHexString);
    }else{
        //1.读取textEdit中的数据
        QString tmpHexString = ui->textEdit->toPlainText();
        qDebug() << "读取的Hex数据String类型:" << tmpHexString;
        //QString转QByteArray
        QByteArray tmpHexByteArray = tmpHexString.toUtf8();
        qDebug() << "转换前的Hex数据Byte类型:" << tmpHexByteArray;
        //2.转换成16进制
        //16进制QByteArray转成ASSIIQByteArray
        QByteArray tmpByteArray = QByteArray::fromHex(tmpHexByteArray);
        qDebug() << "转换后的ASSII数据Byte类型:" << tmpByteArray;
        //3.显示在textEdit中
        //ASSIIQByteArray转成QString
        QString tmpString = QString::fromUtf8(tmpByteArray);
        qDebug() << "转换后的ASSII数据String类型:" << tmpString;
        ui->textEdit->setText(tmpString);
    }
}

void MainWindow::on_hideBoardPushButton_clicked(bool checked)
{
    if (checked){
        qDebug() << "隐藏主窗口: " << checked;
        ui->hideBoardPushButton->setText("拓展面板");
        ui->groupBoxMText->hide();
    }else{
        qDebug() << "显示主窗口: " << checked;
        ui->hideBoardPushButton->setText("隐藏面板");
        ui->groupBoxMText->show();
    }

}

void MainWindow::on_hideHisPushButton_clicked(bool checked)
{
    if (checked){
        qDebug() << "隐藏历史: " << checked;
        ui->hideHisPushButton->setText("拓展历史");
        ui->groupBoxHis->hide();
    }else{
        qDebug() << "显示历史: " << checked;
        ui->hideHisPushButton->setText("隐藏历史");
        ui->groupBoxHis->show();
    }
}

// void MainWindow::on_chuanComboBox_activated(int index)
// {
//     qDebug() << "刷新串口";
//     ui->chuanComboBox->clear();
//     //自动检测可用串口，并添加到comboBox中
//     QList<QSerialPortInfo> serialList = QSerialPortInfo::availablePorts();
//     for (QSerialPortInfo serial : serialList) {
//         qDebug() << serial.portName();
//         ui->chuanComboBox->addItem(serial.portName());
//     }
//     //状态栏
//     QString str = "<b>Refresh Serial Port</b>";
//     ui->statusLabel->setText(str);
// }

//########################一般函数#################################

void MainWindow::getSystemTime()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    myTime = currentTime.toString("yyyy-MM-dd hh:mm:ss");
    qDebug() << "当前系统时间:" << myTime;

}

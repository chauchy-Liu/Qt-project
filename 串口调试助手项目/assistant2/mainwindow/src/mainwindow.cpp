#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QFileDialog>
#include <QThread>


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
    //设置循环发送定时器
    xunhuanTimer = new QTimer(this);
    xunhuanCount = 0;
    //初始化线程对像
    myThread = new CustomThread(this);

    //设置初始化成员变量
    isSerialPortOpen = false;
    isTimeBoxChecked = false;
    
    for (int i=0; i <= 12; i++){
        QString btnName = QString("pushButton%1").arg(i+1);
        //findChild找某类型的控件对象
        QPushButton* btn = findChild<QPushButton*>(btnName);
        if (btn != nullptr) {
            //设置pushbutton属性，为了在槽函数引用ID
            btn->setProperty("pushButtonId", i+1);
            pushButtonList.append(btn);
            //设置信号与槽
            connect(btn, &QPushButton::clicked, this, &MainWindow::on_sendSerialTextCommand);
        }
    }
    //设置循环发送间隔默认值,单位ms
    ui->xunhuanSpinBox->setValue(1000);

    //设置信号与槽
    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::onSerialDataReadyToRead);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_sendPushButton_clicked);
    connect(statusTimer, &QTimer::timeout, this, &MainWindow::refreshStatusTime);
    connect(ui->chuanComboBox, &ComboxEvent::refresh, this, &MainWindow::on_chuanComboBox_refresh);
    connect(xunhuanTimer, &QTimer::timeout, this, &MainWindow::xuhuanSend);
    connect(myThread, &CustomThread::threadTimeOut, this, &MainWindow::xuhuanSend);

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
        //显示在历史ui框中
        if (this->sendHis != revMessage){
            this->sendHis = revMessage;
            ui->textEditHis->append(revMessage);
        }
        //光标位于文本框最新位置
        ui->textEdit->moveCursor(QTextCursor::End);
        ui->textEditHis->moveCursor(QTextCursor::End);
        //光标可视化
        ui->textEdit->ensureCursorVisible();
        ui->textEditHis->ensureCursorVisible();
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
        // ui->textEdit->append(revMessage);
        // this->readCount += data.size();
        // ui->RevLabel->setText(QString("Received: %1").arg(this->readCount)); //setNum(this->readCount);
        // QString revMessage = ui->sendLineEdit->text();
        // qDebug() << "串口对象读取的string数据:" << revMessage;
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
            //显示在历史ui框中
            if (this->sendHis != revMessage){
                this->sendHis = revMessage;
                ui->textEditHis->append(revMessage);
            }
            //光标位于文本框最新位置
            ui->textEdit->moveCursor(QTextCursor::End);
            ui->textEditHis->moveCursor(QTextCursor::End);
            //光标可视化
            ui->textEdit->ensureCursorVisible();
            ui->textEditHis->ensureCursorVisible();
        }
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
        //光标位置
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textEdit->setTextCursor(cursor);
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
        //光标位置
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textEdit->setTextCursor(cursor);
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

void MainWindow::on_chuanComboBox_refresh()
{
    qDebug() << "刷新串口";
    ui->chuanComboBox->clear();
    //自动检测可用串口，并添加到comboBox中
    QList<QSerialPortInfo> serialList = QSerialPortInfo::availablePorts();
    for (QSerialPortInfo serial : serialList) {
        qDebug() << serial.portName();
        ui->chuanComboBox->addItem(serial.portName());
    }
    //状态栏
    QString str = "<b>Refresh Serial Port</b>";
    ui->statusLabel->setText(str);
}

void MainWindow::on_sendSerialTextCommand(){
    //sender()获取信号发出的控件
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    //lineEdit控件
    QString lineEditString = QString("lineEdit") + btn->property("pushButtonId").toString();
    QLineEdit* lineEdit = findChild<QLineEdit*>(lineEditString);
    //checkBox控件
    QString checkBoxString = QString("checkBox") + btn->property("pushButtonId").toString();
    QCheckBox* checkBox = findChild<QCheckBox*>(checkBoxString);
    //业务逻辑
    if (lineEdit->text().size() <= 0){
        return;
    }

    ui->sendLineEdit->setText(lineEdit->text());
    ui->HexSendCheckBox->setChecked(checkBox->isChecked());
    on_sendPushButton_clicked();
}

void MainWindow::on_xunhuanCheckBox_clicked(bool checked)
{
    
    if (ui->xunhuanCheckBox->isChecked()){
        qDebug() << "循环发送数据槽函数, 是否勾选：" << ui->xunhuanCheckBox->isChecked();
        ui->xunhuanSpinBox->setEnabled(false);
        // xunhuanTimer->start(ui->xunhuanSpinBox->value());
        myThread->start();
        
    }else{
        // xunhuanTimer->stop();
        // myThread->deleteLater();
        myThread->terminate();
        ui->xunhuanSpinBox->setEnabled(true);
        xunhuanCount = 0;
    }
}

void MainWindow::xuhuanSend(){
    qDebug() << "循环发送数据定时槽函数";
    if (xunhuanCount < pushButtonList.size()){
        QPushButton* btnTmp = pushButtonList.at(xunhuanCount);
        emit btnTmp->clicked();
        xunhuanCount++;
    }else{
        xunhuanCount = 0;
    }
}

void MainWindow::on_resetPushButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("提示");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("重置列表不可逆，确认是否重置？");
    // msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    QPushButton* yesButton = msgBox.addButton("是", QMessageBox::YesRole);
    QPushButton* noButton = msgBox.addButton("否", QMessageBox::NoRole);
    int ret = msgBox.exec();
    if (msgBox.clickedButton() == yesButton) {
        qDebug() << "重置列表";
        //遍历lineEdit,清空内容
        //遍历checkBox,取消勾选
        for (int i=0; i < pushButtonList.size(); i++){
            QPushButton* btn = pushButtonList.at(i);
            //lineEdit控件
            QString lineEditString = QString("lineEdit") + btn->property("pushButtonId").toString();
            QLineEdit* lineEdit = findChild<QLineEdit*>(lineEditString);
            lineEdit->clear();
            //checkBox控件
            QString checkBoxString = QString("checkBox") + btn->property("pushButtonId").toString();
            QCheckBox* checkBox = findChild<QCheckBox*>(checkBoxString);
            checkBox->setChecked(false);
        }
    } else if (msgBox.clickedButton() == noButton) {
        qDebug() << "取消重置列表";
    }
}

void MainWindow::on_loadPushButton_clicked()
{
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    QString fileName = fileDialog.getSaveFileName(this, tr("Save File"), "./", tr("All Files (*)"));
    QFile file(fileName);
    bool fileFlag = file.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!fileFlag) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file for writing: %1").arg(file.errorString()));
        return;
    }
    QTextStream out(&file);
    for (int i=0; i<pushButtonList.size(); i++){
        QPushButton* btn = pushButtonList.at(i);
        //lineEdit控件
        QString lineEditString = QString("lineEdit") + btn->property("pushButtonId").toString();
        QLineEdit* lineEdit = findChild<QLineEdit*>(lineEditString);
        QString checkBoxString = QString("checkBox") + btn->property("pushButtonId").toString();
        QCheckBox* checkBox = findChild<QCheckBox*>(checkBoxString);
        out << checkBox->isChecked() << ',' << lineEdit->text() << "\n";
    }
    file.close();
}

void MainWindow::on_savePushButton_clicked()
{
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    QString fileName = fileDialog.getOpenFileName(this, tr("Open File"), "./", tr("All Files (*)"));
    QFile file(fileName);
    bool fileFlag = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!fileFlag) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file for writing: %1").arg(file.errorString()));
        return;
    }
    QTextStream in(&file);
    int lineCount = 0;
    while (!in.atEnd() && lineCount < pushButtonList.size()){
        lineCount++;
        QString line = in.readLine();
        QStringList list = line.split(",");
        if (list.size() != 2){
            QMessageBox::warning(this, tr("Error"), tr("文档格式错误"));
            return;
        }
        bool checkBoxChecked = list.at(0).toInt();
        QString lineEditText = list.at(1);
        try{
            //lineEdit控件
            QString lineEditString = QString("lineEdit") + QString::number(lineCount);
            QLineEdit* lineEdit = findChild<QLineEdit*>(lineEditString);
            lineEdit->setText(lineEditText);
            //checkBox控件
            QString checkBoxString = QString("checkBox") + QString::number(lineCount);
            QCheckBox* checkBox = findChild<QCheckBox*>(checkBoxString);
            checkBox->setChecked(checkBoxChecked);
        }catch (std::exception& e){
            QMessageBox::warning(this, tr("Error"), tr("载入文档的数据行超过窗口设定行"));
            return;
        }catch (...){
            QMessageBox::warning(this, tr("Error"), tr("未知错误"));
            return;
        }
    }
    file.close();
}

// void MainWindow::on_xunhuanCheckBox_clicked(bool checked)
// {
//     if (ui->xunhuanCheckBox->isChecked()){
//         qDebug() << "循环发送数据槽函数, 是否勾选：" << ui->xunhuanCheckBox->isChecked();
//         // while (true){
//         for (int i=0; i<pushButtonList.size(); i++){
//             QPushButton* btnTmp = pushButtonList.at(i);
//             emit btnTmp->clicked();
//             QThread::msleep(ui->xunhuanSpinBox->value());//不能在QT的UI线程中延时，否则导致UI页面刷新问题, 解决方案：1）定时器，2）多线程
//         }
//         // }
//     }else{}
// }

// void MainWindow::on_pushButton1_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit1->text());
//     if (ui->checkBox1->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

// void MainWindow::on_pushButton2_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit2->text());
//     if (ui->checkBox2->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

// void MainWindow::on_pushButton3_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit3->text());
//     if (ui->checkBox3->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

// void MainWindow::on_pushButton4_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit4->text());
//     if (ui->checkBox4->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

// void MainWindow::on_pushButton5_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit5->text());
//     if (ui->checkBox5->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

// void MainWindow::on_pushButton6_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit6->text());
//     if (ui->checkBox6->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

// void MainWindow::on_pushButton7_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit7->text());
//     if (ui->checkBox7->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

// void MainWindow::on_pushButton8_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit8->text());
//     if (ui->checkBox8->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

// void MainWindow::on_pushButton9_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit9->text());
//     if (ui->checkBox9->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

// void MainWindow::on_pushButton10_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit10->text());
//     if (ui->checkBox10->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

// void MainWindow::on_pushButton11_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit11->text());
//     if (ui->checkBox11->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

// void MainWindow::on_pushButton12_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit12->text());
//     if (ui->checkBox12->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

// void MainWindow::on_pushButton13_clicked()
// {
//     ui->sendLineEdit->setText(ui->lineEdit13->text());
//     if (ui->checkBox13->isChecked()){
//         ui->HexSendCheckBox->setChecked(true);
//     }else{
//         ui->HexSendCheckBox->setChecked(false);
//     }
//     on_sendPushButton_clicked();
// }

//########################一般函数#################################

void MainWindow::getSystemTime()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    myTime = currentTime.toString("yyyy-MM-dd hh:mm:ss");
    qDebug() << "当前系统时间:" << myTime;

}



#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkInterface>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setLayout(ui->widgetVL);
    // //建立一个客户端
    client = nullptr;//new QTcpSocket(this);
    //客户端可用IP
    QList<QHostAddress> clientAddresses = QNetworkInterface::allAddresses();
    for (QHostAddress address: clientAddresses) {
        //筛选ipv4
        if (address.protocol() == QAbstractSocket::IPv4Protocol) {
            ui->ipClientComboBox->addItem(address.toString());
        }
    }
    //按键不可选
    ui->interruptPushButton->setEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_connectPushButton_clicked() {
    qDebug() << "客户端连接";
    //client的分配不能放到构造函数中，因为当服务端处在未监听状态时client建立的连接会一直处在未连接状态，即使之后服务端处在监听状态，因此需要重新分配一个client删除原有client
    if (!client) {
        client = new QTcpSocket(this);
    }else {
        delete client;
        client = new QTcpSocket(this);
    }
    //信号与槽
    connect(client, &QAbstractSocket::readyRead, this, &MainWindow::on_receiveData_handler);
    connect(client, &QAbstractSocket::stateChanged, this, &MainWindow::on_stateChange_handler);
    //取消已有连接
    // client->abort();
    // client->close();
    // client->disconnectFromHost();
    if (ui->ipClientComboBox->currentText().isEmpty()  || ui->portClientLineEdit->text().isEmpty() || ui->portServerLineEdit->text().isEmpty() || ui->ipServerLineEdit->text().isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("提醒");
        msgBox.setText("请设定客户端和服务端的IP和端口号");
        msgBox.exec();
        return;
    }
    QHostAddress ipClient = QHostAddress(ui->ipClientComboBox->currentText());
    int portClient = ui->portClientLineEdit->text().toInt();
    //socket绑定客户端ip和端口号
    client->bind(ipClient, portClient);
    //连接服务端ip和端口号
    client->connectToHost(ui->ipServerLineEdit->text(), ui->portServerLineEdit->text().toInt());

}

void MainWindow::on_receiveData_handler() {
    qDebug() << "客户端接收服务端的数据";
    //客户端socket读取服务端发送的数据
    QByteArray revData = client->readAll();
    //显示到窗口
    // ui->showTextEdit->insertPlainText(QString::fromUtf8(revData)+"\n");
    QTextCursor cursor = ui->showTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->showTextEdit->setAlignment(Qt::AlignLeft);
    QTextCharFormat format;
    format.setForeground(QBrush(QColor(Qt::black)));
    cursor.setCharFormat(format);
    cursor.insertText(QString::fromUtf8(revData)+"\n");
    // ui->showTextEdit->moveCursor(QTextCursor::End);
    // ui->showTextEdit->setTextCursor(ui->showTextEdit->textCursor());
    //光标位置
    // QTextCursor cursor = ui->showTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->showTextEdit->setTextCursor(cursor);
    ui->showTextEdit->ensureCursorVisible();
}

void MainWindow::on_sendPushButton_clicked() {
    qDebug() << "客户端发送数据";
    client->write(ui->sendTextEdit->toPlainText().toUtf8());
    QTextCursor cursor = ui->showTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->showTextEdit->setAlignment(Qt::AlignRight);
    QTextCharFormat format;
    format.setForeground(QBrush(QColor(Qt::red)));
    cursor.setCharFormat(format);
    cursor.insertText(ui->sendTextEdit->toPlainText()+"\n");
    // ui->showTextEdit->insertPlainText(ui->sendTextEdit->toPlainText());
}

void MainWindow::on_interruptPushButton_clicked() {
    qDebug() << "客户端断开连接";
    QTextCursor cursor = ui->showTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->showTextEdit->setAlignment(Qt::AlignRight);
    QTextCharFormat format;
    format.setForeground(QBrush(QColor(Qt::red)));
    cursor.setCharFormat(format);
    cursor.insertText(QString("客户端断开连接!\n"));
    client->close();
    // ui->showTextEdit->append("客户端断开连接!");
    // ui->showTextEdit->moveCursor(QTextCursor::End);
    // ui->showTextEdit->setTextCursor(ui->showTextEdit->textCursor());
    //光标位置
    // QTextCursor cursor = ui->showTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->showTextEdit->setTextCursor(cursor);
    ui->showTextEdit->ensureCursorVisible();
    //设定不可选按钮
    ui->ipClientComboBox->setEnabled(true);
    ui->portClientLineEdit->setEnabled(true);
    ui->ipServerLineEdit->setEnabled(true);
    ui->portServerLineEdit->setEnabled(true);
    ui->interruptPushButton->setEnabled(false);
    ui->connectPushButton->setEnabled(true);
}

void MainWindow::on_stateChange_handler() {
    qDebug() << "客户端和服务端的连接状态发生改变";
    if (client->state() == QAbstractSocket::UnconnectedState) {
        ui->showTextEdit->append("服务端断开连接!\n");
        //光标位置
        QTextCursor cursor = ui->showTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->showTextEdit->setTextCursor(cursor);
        ui->showTextEdit->ensureCursorVisible();
        //设定不可选按钮
        ui->ipClientComboBox->setEnabled(true);
        ui->portClientLineEdit->setEnabled(true);
        ui->ipServerLineEdit->setEnabled(true);
        ui->portServerLineEdit->setEnabled(true);
        ui->interruptPushButton->setEnabled(false);
        ui->connectPushButton->setEnabled(true);
        //如果删除socket会编程空指针，在用client时内存访问出错
        // client->deleteLater();
        // client->abort();
        client->disconnectFromHost();
        client->close();
    }else if (client->state() == QAbstractSocket::ClosingState){
        ui->showTextEdit->append("正在断开服务端连接!\n");
    }else if(client->state() == QAbstractSocket::ConnectedState) {
        ui->showTextEdit->append(QString("连接成功!\n").arg(ui->ipServerLineEdit->text()).arg(ui->portServerLineEdit->text()));
        // ui->showTextEdit->moveCursor(QTextCursor::End);
        // ui->showTextEdit->setTextCursor(ui->showTextEdit->textCursor());
        //光标位置
        QTextCursor cursor = ui->showTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->showTextEdit->setTextCursor(cursor);
        ui->showTextEdit->ensureCursorVisible();
        //设定不可选按钮
        ui->ipClientComboBox->setEnabled(false);
        ui->portClientLineEdit->setEnabled(false);
        ui->ipServerLineEdit->setEnabled(false);
        ui->portServerLineEdit->setEnabled(false);
        ui->interruptPushButton->setEnabled(true);
        ui->connectPushButton->setEnabled(false);
    }else if (client->state() == QAbstractSocket::ConnectingState) {
        ui->showTextEdit->append(QString("正连接服务端IP: %1, 端口: %2\n").arg(ui->ipServerLineEdit->text()).arg(ui->portServerLineEdit->text()));
    }
}
//##################################################
//成员函数
void MainWindow::setInsertChartFormat(QTextEdit *textEdit, Qt::GlobalColor color, Qt::AlignmentFlag align) {
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEdit->setAlignment(align);
    QTextCharFormat format;
    format.setForeground(QBrush(QColor(color)));
    cursor.setCharFormat(format);
    cursor.insertText(textEdit->toPlainText()+"\n");
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QMessageBox>

#include "../../customWidget/include/socketComboBox.h"
#include <QMetaObject>
#include <QMetaEnum>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->centralwidget->setLayout(ui->widgetVL);

    //
    server = new QTcpServer(this);
    // //监听
    // QHostAddress host("192.168.1.9");
    // if (!server->listen(host, 8888)) {
    //     qDebug() << "Server listen failed";
    // }
    //按键初始设置为不可按
    ui->stopPushButton->setEnabled(false);
    ui->interruptPushButton->setEnabled(false);
    ui->sendPushButton->setEnabled(false);
    ui->socketComboBox->setEnabled(false);
    //服务器端可用的ip地址
    QList<QHostAddress> serverAddresses = QNetworkInterface::allAddresses();
    for (QHostAddress address: serverAddresses) {
        //筛选ipv4
        if (address.protocol() == QAbstractSocket::IPv4Protocol) {
            ui->ipComboBox->addItem(address.toString());
        }
    }

    //信号与槽
    connect(server, &QTcpServer::newConnection, this, &MainWindow::on_newClient_connect);
    connect(ui->socketComboBox, &SocketComboBox::on_ComboBox_clicked, this, &MainWindow::on_socketComboBox_handler);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_newClient_connect() {
    qDebug() << "有新客户端连接socket";
    if (server->hasPendingConnections()) {
        QTcpSocket *socket = server->nextPendingConnection();
        qDebug() << "Client host: " << socket->peerAddress().toString() << " port: " << socket->peerPort();
        ui->showTextEdit->insertPlainText(
            "客户端地址：" + socket->peerAddress().toString() + ", 客户端端口：" + QString::number(socket->peerPort()) + "\n");
        // ui->showTextEdit->moveCursor(QTextCursor::End);
        // ui->showTextEdit->setTextCursor(ui->showTextEdit->textCursor());
        //光标位置
        QTextCursor cursor = ui->showTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->showTextEdit->setTextCursor(cursor);
        ui->showTextEdit->ensureCursorVisible();
        //添加新socket到comboBox
        if (ui->socketComboBox->count() == 0) {
            ui->socketComboBox->addItem("all");
        }
        ui->socketComboBox->addItem(socket->peerAddress().toString()+":"+QString::number(socket->peerPort()));
        // 设置默认项为第一个选项（索引为0）
        ui->socketComboBox->setCurrentIndex(ui->socketComboBox->count()-1);
        socketIndex = ui->socketComboBox->currentIndex();
        //信号与槽：接收数据
        connect(socket, &QTcpSocket::readyRead, this, &MainWindow::on_readyRead_handler);
        //发送pushButton可按
        ui->sendPushButton->setEnabled(true);
        //信号与槽：检测socket连接
        // connect(socket, &QAbstractSocket::disconnected, this, &MainWindow::on_disconnected_handler);
        //信号与槽：检测socket状态
        connect(socket, &QAbstractSocket::stateChanged, this, &MainWindow::on_socketStateChanged_handler);
    }else {
        ui->sendPushButton->setEnabled(false);
    }
}

void MainWindow::on_startPushButton_clicked() {
    qDebug() << "服务器开始监听";
    //监听
    int port = ui->portLineEdit->text().toInt();
    // QHostAddress host("192.168.1.9");
    //监听所有ip
    // if (!server->listen(QHostAddress::Any, port)) {
    //只监听combox中的ip
    QHostAddress host(ui->ipComboBox->currentText());
    if (!server->listen(host, port)) {
        qDebug() << "Server listen failed";
        QMessageBox msgBox;
        msgBox.setWindowTitle("监听失败");
        msgBox.setText("端口号被占用");
        msgBox.exec();
        return;
    }
    //按键设置为可按
    ui->stopPushButton->setEnabled(true);
    ui->interruptPushButton->setEnabled(true);
    //按键设置为不可按
    ui->startPushButton->setEnabled(false);
    ui->protocolComboBox->setEnabled(false);
    ui->ipComboBox->setEnabled(false);
    ui->portLineEdit->setEnabled(false);
    ui->socketComboBox->setEnabled(true);
}

void MainWindow::on_readyRead_handler() {
    qDebug() << "在socket连接中接收客服端数据";
    //获取socket连接
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    //读发送的数据
    QByteArray revData = socket->readAll();
    //显示到窗口
    // ui->showTextEdit->insertPlainText(QString::fromUtf8(revData) + "\n");
    QTextCursor cursor = ui->showTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->showTextEdit->setAlignment(Qt::AlignLeft);
    QTextCharFormat format;
    format.setForeground(QBrush(QColor(Qt::red)));
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

// void MainWindow::on_disconnected_handler() {
//     qDebug() << "disconnected信号引起的客户端socket断开";
//     ui->showTextEdit->insertPlainText("disconnected信号引起的客户端断开\n");
//     // ui->showTextEdit->moveCursor(QTextCursor::End);
//     // ui->showTextEdit->setTextCursor(ui->showTextEdit->textCursor());
//     //光标位置
//     QTextCursor cursor = ui->showTextEdit->textCursor();
//     cursor.movePosition(QTextCursor::End);
//     ui->showTextEdit->setTextCursor(cursor);
//     //释放socket资源
//     QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
//     socket->deleteLater();
// }

void MainWindow::on_socketStateChanged_handler(QAbstractSocket::SocketState socketState) {
    QMetaObject  mo = QAbstractSocket::staticMetaObject;
    QMetaEnum me = mo.enumerator(mo.indexOfEnumerator("SocketState"));
    qDebug() << "客户端socket状态改变: " + QString(me.valueToKey(socketState));
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socketState == QAbstractSocket::UnconnectedState || socketState == QAbstractSocket::ClosingState) {
        // ui->showTextEdit->insertPlainText("socketStateChanged信号引起的客户端断开\n");
        QTextCursor cursor = ui->showTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->showTextEdit->setAlignment(Qt::AlignLeft);
        QTextCharFormat format;
        format.setForeground(QBrush(QColor(Qt::red)));
        cursor.setCharFormat(format);
        cursor.insertText( QString(me.valueToKey(socketState))+QString("信号引起的客户端断开\n"));
        if (socketState == QAbstractSocket::UnconnectedState) {
            QString deleteSocket = socket->peerAddress().toString()+':'+QString::number(socket->peerPort());
            int deleteIndex = ui->socketComboBox->findText(deleteSocket);
            if (deleteIndex != -1) {
                ui->socketComboBox->removeItem(deleteIndex);
            }
            // QList<QTcpSocket*> clientSockets = server->findChildren<QTcpSocket*>();
            // if (clientSockets.size() == 0) {
            //     ui->socketComboBox->clear();
            // }
            if (ui->socketComboBox->count() == 1) {
                ui->socketComboBox->clear();
                ui->sendPushButton->setEnabled(false);
            }
        }
        socket->deleteLater();
    } else if (socketState == QAbstractSocket::ConnectedState || socketState == QAbstractSocket::ConnectingState) {
        // ui->showTextEdit->insertPlainText("socketStateChanged信号引起的客户端连接\n");
        QTextCursor cursor = ui->showTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->showTextEdit->setAlignment(Qt::AlignLeft);
        QTextCharFormat format;
        format.setForeground(QBrush(QColor(Qt::red)));
        cursor.setCharFormat(format);
        cursor.insertText( QString(me.valueToKey(socketState))+QString("信号引起的客户端连接\n"));
    }
    // ui->showTextEdit->moveCursor(QTextCursor::End);
    // ui->showTextEdit->setTextCursor(ui->showTextEdit->textCursor());
    //光标位置
    QTextCursor cursor = ui->showTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->showTextEdit->setTextCursor(cursor);
    ui->showTextEdit->ensureCursorVisible();
}

void MainWindow::on_sendPushButton_clicked() {
    qDebug() << "服务端发送";
    QList<QTcpSocket *> tcpSocketClients = server->findChildren<QTcpSocket *>();
    if (tcpSocketClients.size() <= 0) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("发送错误");
        msgBox.setText("未指定客户端连接");
        msgBox.exec();
        return;
    }
    if (socketIndex == 0) {
        for (QTcpSocket *tcpSocketClient: tcpSocketClients) {
            tcpSocketClient->write(ui->sendTextEdit->toPlainText().toUtf8());
            QTextCursor cursor = ui->showTextEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->showTextEdit->setAlignment(Qt::AlignRight);
            QTextCharFormat format;
            format.setForeground(QBrush(QColor(Qt::black)));
            cursor.setCharFormat(format);
            cursor.insertText(ui->sendTextEdit->toPlainText()+"\n");
        }
    } else {
        tcpSocketClients[socketIndex - 1]->write(ui->sendTextEdit->toPlainText().toUtf8());
        QTextCursor cursor = ui->showTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->showTextEdit->setAlignment(Qt::AlignRight);
        QTextCharFormat format;
        format.setForeground(QBrush(QColor(Qt::black)));
        cursor.setCharFormat(format);
        cursor.insertText(ui->sendTextEdit->toPlainText()+"\n");
    }
}

void MainWindow::on_socketComboBox_handler() {
    qDebug() << "筛选目标socket";
    //清除原有项
    ui->socketComboBox->clear();
    if (server->isListening()) {
        QList<QTcpSocket *> tcpSocketClients = server->findChildren<QTcpSocket *>();
        if (tcpSocketClients.size() <= 0) {
            ui->sendPushButton->setEnabled(false);
            QMessageBox msgBox;
            msgBox.setWindowTitle("无客户端连接");
            msgBox.setText("服务端没有监听到客户端socket连接");
            msgBox.exec();
            return;
        }
        //可以发送
        ui->sendPushButton->setEnabled(true);

        //可用连接
        ui->socketComboBox->addItem("all");
        for (QTcpSocket *tcpSocketClient: tcpSocketClients) {
            ui->socketComboBox->addItem(tcpSocketClient->peerAddress().toString() + ':' + QString::number(tcpSocketClient->peerPort()));
        }
    }else {
        ui->sendPushButton->setEnabled(false);
    }


}

void MainWindow::on_socketComboBox_activated(int index) {
    socketIndex = index;
}

void MainWindow::on_stopPushButton_clicked() {
    qDebug() << "服务器断开连接";
    //客户端回收
    if (server->isListening()) {
        QList<QTcpSocket *> tcpSocketClients = server->findChildren<QTcpSocket *>();
        for (QTcpSocket *tcpSocketClient: tcpSocketClients) {
            tcpSocketClient->close();
            // tcpSocketClient->deleteLater();
        }
        //关闭服务端
        server->close();
    }
    ui->startPushButton->setEnabled(true);
    ui->stopPushButton->setEnabled(false);
    ui->interruptPushButton->setEnabled(false);
    ui->protocolComboBox->setEnabled(true);
    ui->ipComboBox->setEnabled(true);
    ui->portLineEdit->setEnabled(true);
    ui->sendPushButton->setEnabled(false);
    ui->socketComboBox->clear();
    ui->socketComboBox->setEnabled(false);
}

void MainWindow::on_interruptPushButton_clicked() {
    qDebug() << "退出程序";
    on_stopPushButton_clicked();
    delete server;
    this->close();
}

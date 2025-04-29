#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QTextCodec>
#include <QMessageBox>
#include <QShortcut>
#include <QWheelEvent>
#include "mybutton.h"
#include "myTextEdit.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //快捷键
    QShortcut* openShortcut = new QShortcut(QKeySequence(tr("Ctrl+O", "File|Open")), this);
    QShortcut* saveShortcut = new QShortcut(QKeySequence(tr("Ctrl+S", "File|Save")), this);
    QShortcut* zoomInShortcut = new QShortcut(QKeySequence(tr("Ctrl+Shift+=")), this);
    QShortcut* zoomOutShortcut = new QShortcut(QKeySequence(tr("Ctrl+-")), this);
    connect(openShortcut, &QShortcut::activated, this, &MainWindow::on_btnOpenIcon_clicked);
    connect(saveShortcut, &QShortcut::activated, this, &MainWindow::on_btnSaveIcon_clicked);
    // preEncodeName = ui->encodeComboBox->currentText();
    connect(zoomInShortcut, &QShortcut::activated, this, [=](){
        zoomIn();
    });
    connect(zoomOutShortcut, &QShortcut::activated, this, [=](){
        zoomOut();
    });
    //自定义按键的信号与槽
    connect(ui->mybutton, &MyButton::clicked, [=](){
        qDebug() << "MyButton Clicked";
    });
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
    QObject::connect(ui->encodeComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::oncurrentIndexChanged);
    QObject::connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::onCursorPositionChanged);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//第一种连接信号与槽方式
void MainWindow::on_btnCloseIcon_clicked()
{
    std::cout << "Close Icon Clicked: on_objectName_signalName format" << std::endl;
    // this->close();
    QMessageBox msgBox;
    if (file.isOpen()){
        msgBox.setWindowTitle(file.fileName());
    }
    msgBox.setText("文件已被修改。");
    msgBox.setInformativeText("你要保存更改吗?");
    //文件路径：1）res.qrc配置的虚拟引用路径":icon/res/notebook.png" 2）真实路径"/resource/res/notebook.png"， “：”表示这是一个Qt资源系统路径，而不是普通的文件系统路径。
    msgBox.setIconPixmap(QPixmap(":/icon/res/sketchbook.png").scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Save, "保存");
    msgBox.setButtonText(QMessageBox::Cancel, "取消");
    msgBox.setButtonText(QMessageBox::Discard, "放弃");
    msgBox.setDefaultButton(QMessageBox::Save);
    // msgBox.addButton("保存", QMessageBox::AcceptRole);
    // msgBox.addButton("取消", QMessageBox::RejectRole);
    // msgBox.addButton("不保存", QMessageBox::DestructiveRole);
    int ret = msgBox.exec();
    switch (ret){
        case QMessageBox::Save: //QMessageBox::AcceptRole:
            // Save was clicked
            qDebug() << "Save was clicked";
            this->on_btnSaveIcon_clicked();
            // file.close();
            break;
        case QMessageBox::Discard: //QMessageBox::DestructiveRole:
            // Don't Save was clicked
            qDebug() << "Don't Save was clicked";
            ui->textEdit->clear();
            this->setWindowTitle("传玺记事本");
            if (file.isOpen()){
                ui->textEdit->clear();
                file.close();
            }else{
                ui->textEdit->clear();
            }
            break;
        case QMessageBox::Cancel://QMessageBox::RejectRole:
            // Cancel was clicked
            qDebug() << "Cancel was clicked";
            return;
    }
    return;
}

void MainWindow::on_btnOpenIcon_clicked(){
    
    std::cout << "分步骤打开" << std::endl;
    QFileDialog  qFileDialog;
    qFileDialog.setWindowTitle(tr("Open File"));
    qFileDialog.setDirectory("/Users/liuchuanxi/项目/QT");
    qFileDialog.setFileMode(QFileDialog::ExistingFiles);
    qFileDialog.setNameFilter(tr("Images (*.png *.jpg *.bmp);;Text files (*.txt)"));
    if (qFileDialog.exec()){
        QStringList fileNames = qFileDialog.selectedFiles();
        for (const QString &fileName : fileNames) {
            std::cout << "Selected File: " << fileName.toStdString() << std::endl;
            // QFile file(fileName);
            this->setWindowTitle(fileName+"--传玺记事本");
            file.setFileName(fileName);
            bool open_flag = file.open(QIODevice::ReadWrite | QIODevice::Text);
            if (!open_flag){
                qDebug() << "Open File Failed!";
                return;
            }//if
            // QTextStream in(&file);
            // in.setEncoding(QStringConverter::Utf8);//("UTF-8");
            // QTextCodec *codec = QTextCodec::codecForName("GBK");
            QString codecName = ui->encodeComboBox->currentText();
            const char* codecNameCharPtr = codecName.toStdString().c_str();
            QTextCodec *codec = QTextCodec::codecForName(codecNameCharPtr);
            int context = file.size();
            qDebug() << "File size: " << context;
            
            //窗口清除原有文本
            ui->textEdit->clear();
            // while(!in.atEnd()){
            while(!file.atEnd()){
                // QString line = in.readLine();
                QByteArray line = file.readLine();
                QString line_code =  codec->toUnicode(line);
                qDebug() << "File Content: " << line;
                qDebug() << "编码 File Content: " << line_code;
                if (file.pos() == line.length()+1){
                    //覆盖内容
                    ui->textEdit->setText(line_code);
                }else{
                    //追加内容
                    ui->textEdit->append(line_code);
                }//end if
            }//while
            // file.close();
        }//for
    } else {
        std::cout << "No File Selected" << std::endl;
        return;
    }//if
}
void MainWindow::on_btnSaveIcon_clicked(){
    std::cout << "分步骤保存" << std::endl;
    //未打开的文件需要对话框
    if (!file.isOpen()){
        QFileDialog  qFileDialog;
        qFileDialog.setWindowTitle(tr("Open File"));
        qFileDialog.setDirectory("/Users/liuchuanxi/项目/QT");
        qFileDialog.setAcceptMode(QFileDialog::AcceptSave);  // 设置为保存模式
        qFileDialog.setDefaultSuffix("txt");                 // 默认后缀
        qFileDialog.setFileMode(QFileDialog::AnyFile);
        qFileDialog.setNameFilter(tr("Text files (*.txt)"));
        if (qFileDialog.exec() == QFileDialog::Accepted){
            QString fileName = qFileDialog.selectedFiles().first();
            qDebug() <<"保存的文件："<< fileName;
            // QFile file(fileName);
            this->setWindowTitle(fileName+"--传玺记事本");
            file.setFileName(fileName);
            bool open_flag = file.open(QIODevice::WriteOnly | QIODevice::Text);
            if (!open_flag){
                qDebug() << "Open File Failed!";
                return;
            }
            // QTextStream out(&file);
            // out.setEncoding(QStringConverter::Utf8);
            QString codecName = ui->encodeComboBox->currentText();
            const char* codecNameCharPtr = codecName.toStdString().c_str();
            QTextCodec *codec = QTextCodec::codecForName(codecNameCharPtr);
            //从窗口中提取内容
            QString context = ui->textEdit->toPlainText();
            QByteArray out = codec->fromUnicode(context);
            qint64 writeFlag = file.write(out);
            file.flush();  // 强制刷新缓冲区
            // file.close();
        } else {
            std::cout << "No File Selected" << std::endl;
            return;
        }
    } else {
        //清空原文件
        QString fileName = file.fileName();
        file.close();
        file.setFileName(fileName);
        bool open_flag = file.open(QIODevice::WriteOnly | QIODevice::Text);
        if (!open_flag){
            qDebug() << "Open File Failed!";
            return;
        }
        //重新写入窗口中的文字
        QString codecName = ui->encodeComboBox->currentText();
        const char* codecNameCharPtr = codecName.toStdString().c_str();
        QTextCodec *codec = QTextCodec::codecForName(codecNameCharPtr);
        //从窗口中提取内容
        QString context = ui->textEdit->toPlainText();
        QByteArray out = codec->fromUnicode(context);
        qint64 writeFlag = file.write(out);
        file.flush();  // 强制刷新缓冲区
        int a = 1;
    }//end if
    
}
void MainWindow::btnOldConnectFunc()
{
    std::cout << "Old Connect Button Clicked: QObject::connect(sender, SIGNAL(signal), receiver, SLOT(slot));" << std::endl;
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/Users/liuchuanxi/项目/QT", tr("Images (*.png *.jpg *.bmp);;Text files (*.txt)"));
    qDebug() << fileName;
    // QFile file(fileName);
    file.setFileName(fileName);
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
    file.close();
}

void MainWindow::btnNewConnectFunc()
{
    std::cout << "New Connect Button Clicked: QObject::connect(sender, &Sender::signal, receiver, &Receiver::slot);" << std::endl;
    qDebug() << "静态方法实现保存文件";
    QString fileName = QFileDialog::getSaveFileName(this, 
        tr("Save File"), "/Users/liuchuanxi/项目/QT/untitled.txt", tr("Text files (*.txt)"));
    qDebug() <<"保存的文件："<< fileName;
    // QFile file(fileName);
    file.setFileName(fileName);
    bool open_flag = file.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!open_flag){
        qDebug() << "Open File Failed!";
        return;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "这是保存后自动生成的文字";
    file.close();
}

void MainWindow::oncurrentIndexChanged(int index){
    qDebug() << index;
    qDebug() <<"当前编码：" << ui->encodeComboBox->currentText();
    ui->textEdit->clear();
    if (file.isOpen()){
        qDebug() << "文件已打开";
        ui->textEdit->clear();
        QTextCodec* codec = QTextCodec::codecForName(ui->encodeComboBox->currentText().toStdString().c_str());
        file.seek(0);
        while (!file.atEnd()){
            QByteArray line = file.readLine();
            QString line_code = codec->toUnicode(line);
            qDebug() << "编码 File Content: " << line_code;
            if (file.pos() == line.length()+1){
                //覆盖内容
                ui->textEdit->setText(line_code);
            }else{
                //追加内容
                ui->textEdit->append(line_code);
            }//end if
        }
    }

}
void MainWindow::onCursorPositionChanged(){
    QTextCursor cursor = ui->textEdit->textCursor();
    qDebug() << "列: " << cursor.columnNumber()+1 << "行: " << cursor.blockNumber()+1;
    int row = cursor.blockNumber()+1;
    int column = cursor.columnNumber()+1;
    ui->posLabel->setText(QString::number(row)+"行，"+QString::number(column)+"列");
    //设置当前行高亮
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    //知道当前行行号
    selection.cursor = ui->textEdit->textCursor();
    //颜色
    // QColor color = 
    QBrush qBrush(Qt::yellow);
    selection.format.setBackground(qBrush);
    //让整行亮
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    //设置
    extraSelections.append(selection);
    ui->textEdit->setExtraSelections(extraSelections);
}

void MainWindow::zoomIn(){
    qDebug() << "Zoom In";
    //获取当前字体信息
    QFont font = ui->textEdit->font();
    //获取当前字体大小
    int fontSize = font.pointSize();
    //改变当前字体大小
    if (fontSize==-1) return;
    int newFontSize = fontSize + 1;
    font.setPointSize(newFontSize);
    ui->textEdit->setFont(font);
    // ui->textEdit->setFontPointSize(fontSize+1);
}
void MainWindow::zoomOut(){
    qDebug() << "Zoom Out";
    //获取当前字体信息
    QFont font = ui->textEdit->font();
    //获取当前字体大小
    int fontSize = font.pointSize();
    //改变当前字体大小
    if (fontSize==-1) return;
    int newFontSize = fontSize - 1;
    font.setPointSize(newFontSize);
    ui->textEdit->setFont(font);
    // ui->textEdit->setFontPointSize(fontSize-1);
}

void MainWindow::enterEvent(QEnterEvent *event)
{
    qDebug() << "mouse enter";
}

void MainWindow::leaveEvent(QEvent *event)
{
    qDebug() << "mouse leave";
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    qDebug() << "MainWindow.cpp wheelEvent";
    qDebug() << event->angleDelta();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "close widget";
    QMessageBox msgBox;
    if (file.isOpen()){
        msgBox.setWindowTitle(file.fileName());
    }
    msgBox.setText("记事本关闭");
    msgBox.setInformativeText("你要关闭窗口吗?");
    //文件路径：1）res.qrc配置的虚拟引用路径":icon/res/notebook.png" 2）真实路径"/resource/res/notebook.png"， “：”表示这是一个Qt资源系统路径，而不是普通的文件系统路径。
    msgBox.setIconPixmap(QPixmap(":/icon/res/sketchbook.png").scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, "关闭");
    msgBox.setButtonText(QMessageBox::No, "不关闭");
    msgBox.setDefaultButton(QMessageBox::Yes);
    // msgBox.addButton("保存", QMessageBox::AcceptRole);
    // msgBox.addButton("取消", QMessageBox::RejectRole);
    // msgBox.addButton("不保存", QMessageBox::DestructiveRole);
    int ret = msgBox.exec();
    switch(ret){
        case QMessageBox::Yes: //QMessageBox::AcceptRole:
            // Save was clicked
            qDebug() << "Yes was clicked";
            event->accept();
            break;
        case QMessageBox::No: //QMessageBox::DestructiveRole:
            // Don't Save was clicked
            qDebug() << "No was clicked";
            event->ignore();
            break;
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    qDebug() << "resize widget:" << "old size:" << event->oldSize() << "new size:" << event->size();
}

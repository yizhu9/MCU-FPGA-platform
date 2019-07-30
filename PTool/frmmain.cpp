#include "frmmain.h"
#include "ui_frmmain.h"
#include "quiwidget.h"
#include "qextserialport.h"

#define TextWidth 1
#define LineWidth 1.5
#define DotWidth 3
#define TextColor QColor(255,255,255)

frmMain::frmMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();
    this->initPlot();
    this->initConfig();
    this->changeResult();
    QUIHelper::setFormInCenter(this);
    on_btnOpen_clicked();
}

frmMain::~frmMain()
{
    delete ui;
}

qint16 frmMain::strHexToShort(const QString &strHex)
{
    bool ok;
    return strHex.toUShort(&ok, 16);
}

void frmMain::initForm()
{
    QStringList portName;
    for (int i = 1; i <= 20; i++) {
        portName << QString("COM%1").arg(i);
    }
    ui->cboxPortName->addItems(portName);

    QStringList baudRate;
    baudRate << "115200" << "460800" << "921600";
    ui->cboxBaudRate->addItems(baudRate);

    isOpen = false;
    isStart = false;
    buffer.clear();
    lastTime = QDateTime::currentDateTime();
    com = 0;

    timerRead = new QTimer(this);
    timerRead->setInterval(100);
    connect(timerRead, SIGNAL(timeout()), this, SLOT(readData()));
    timerRead->start();

    pageCount = 0;
    pageCurrent = 0;
}

void frmMain::initPlot()
{
    //ui->widgetPlot->xAxis->setAutoTickStep(false);
    //ui->widgetPlot->xAxis->setTickStep(50);
    //ui->widgetPlot->xAxis->setRange(0, 1024);

    //ui->widgetPlot->yAxis->setAutoTickStep(false);
    //ui->widgetPlot->yAxis->setTickStep(1000);
    //ui->widgetPlot->yAxis->setRange(0, 16383);

    ui->widgetPlot->yAxis->setAutoTickCount(10);
    ui->widgetPlot->xAxis->grid()->setSubGridVisible(true);
    //ui->widgetPlot->yAxis2->setVisible(true);

    ui->widgetPlot->xAxis->setTickLabelColor(TextColor);
    ui->widgetPlot->yAxis->setTickLabelColor(TextColor);
    ui->widgetPlot->yAxis2->setTickLabelColor(TextColor);
    ui->widgetPlot->xAxis->setBasePen(QPen(TextColor, TextWidth));
    ui->widgetPlot->yAxis->setBasePen(QPen(TextColor, TextWidth));
    ui->widgetPlot->yAxis2->setBasePen(QPen(TextColor, TextWidth));
    ui->widgetPlot->xAxis->setTickPen(QPen(TextColor, TextWidth));
    ui->widgetPlot->yAxis->setTickPen(QPen(TextColor, TextWidth));
    ui->widgetPlot->yAxis2->setTickPen(QPen(TextColor, TextWidth));
    ui->widgetPlot->xAxis->setSubTickPen(QPen(TextColor, TextWidth));
    ui->widgetPlot->yAxis->setSubTickPen(QPen(TextColor, TextWidth));
    ui->widgetPlot->yAxis2->setSubTickPen(QPen(TextColor, TextWidth));

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    ui->widgetPlot->setBackground(plotGradient);

    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    ui->widgetPlot->axisRect()->setBackground(axisRectGradient);

    ui->widgetPlot->addGraph();
    ui->widgetPlot->graph(0)->setPen(QPen(QColor(41, 138, 220), LineWidth));
    ui->widgetPlot->graph(0)->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ssCircle,
                        QPen(QColor(5, 189, 251), LineWidth),
                        QBrush(QColor(5, 189, 251)), DotWidth));

    ui->widgetPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->widgetPlot->replot();
}

void frmMain::initConfig()
{
    ui->cboxPortName->setCurrentIndex(ui->cboxPortName->findText(QUIConfig::PortName));
    connect(ui->cboxPortName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxBaudRate->setCurrentIndex(ui->cboxBaudRate->findText(QString::number(QUIConfig::BaudRate)));
    connect(ui->cboxBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtMv->setText(QString::number(QUIConfig::Mv));
    connect(ui->txtMv, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtWidth->setText(QString::number(QUIConfig::Width));
    connect(ui->txtWidth, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtCycle->setText(QString::number(QUIConfig::Cycle));
    connect(ui->txtCycle, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtPoint->setText(QString::number(QUIConfig::Point));
    connect(ui->txtPoint, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
}

void frmMain::saveConfig()
{
    QUIConfig::PortName = ui->cboxPortName->currentText();
    QUIConfig::BaudRate = ui->cboxBaudRate->currentText().toInt();
    QUIConfig::Mv = ui->txtMv->text().trimmed().toInt();
    QUIConfig::Width = ui->txtWidth->text().trimmed().toInt();
    QUIConfig::Cycle = ui->txtCycle->text().trimmed().toInt();
    QUIConfig::Point = ui->txtPoint->text().trimmed().toInt();
    QUIConfig::writeConfig();
    changeResult();
}

void frmMain::changeResult()
{
    //自动更新结果值
    ui->labMvResult->setText(QString("结果 = %1 mv").arg(QUIConfig::Mv * ((qreal)1000 / 4096)));
    ui->labWidthResult->setText(QString("结果 = %1 μs").arg(QUIConfig::Width));
    ui->labCycleResult->setText(QString("结果 = %1 μs").arg(QUIConfig::Cycle * 10));
    ui->labPointResult->setText(QString("结果 = %1 采样点").arg(QUIConfig::Point * 1024));
}

void frmMain::loadPlot()
{
    plot_keys.clear();
    plot_values.clear();

#if 0
    //如果不是2的倍数则长度要减去1
    if (currentCount % 2 != 0) {
        currentCount = (currentCount - 1);
    }

    for (int i = 0; i < currentCount / 2; i++) {
        plot_keys.append(i);
    }

    for (int i = 0; i < currentCount; i = i + 2) {
        qint16 value = strHexToShort(QString("%1%2").arg(plot_data.at(i)).arg(plot_data.at(i + 1)));
        plot_values.append(value);
    }

    ui->widgetPlot->graph(0)->setData(plot_keys, plot_values);
    ui->widgetPlot->graph(0)->rescaleAxes();
    ui->widgetPlot->replot();
#else
    for (int i = 0; i < currentCount; i++) {
        QString str = plot_data.at(i);
        plot_keys.append(i);
        plot_values.append(str.toDouble());
    }

    //根据点数自动分页
    int pointCount = currentCount / 1024;
    pageCurrent = 1;
    pageCount = pointCount / 20;
    if (pointCount % 20 != 0) {
        pageCount++;
    }

    showData();
#endif
}

void frmMain::readData()
{
    if (!isOpen || com->bytesAvailable() <= 0) {
        return;
    }

    QByteArray data = com->readAll();
    int len = data.length();
    if (len <= 0) {
        return;
    }

    //取出头部尾部数据判断
    quint8 head = data.at(4);
    quint8 end = data.at(len - 1);
    if (!isStart && head == 0x02) {
        buffer.clear();
        isStart = true;
    }

    //处于接收数据期间不断追加数据
    if (isStart) {
        buffer.append(data);
    }

    //如果buffer的数据大小和预定的采样点数一致则说明数据接收完成
    quint32 bufferLen = buffer.length();
    //每个采样点有两个字节,前面多4个字节+头尾2个字节
    quint32 targetLen = (QUIConfig::Point * 1024 * 2) + 2 + 4;

    //qDebug() << TIMEMS << "接收数据:" << QUIWidget::byteArrayToHexStr(data);
    qDebug() << TIMEMS << "头部数据:" << head << "\t尾部数据:" << end << "\t当前长度:" << bufferLen << "\t目标长度:" << targetLen;

    if (bufferLen == targetLen) {
        checkData();
    }
}

void frmMain::writeData()
{
    pageCount = 0;
    pageCurrent = 0;

    QByteArray data;

    //开始符号
    data.append(0x02);

    //电压幅度
    QString mv = QString("%1").arg(ui->txtMv->text().toInt(), 4, 10, QChar('0'));
    data.append(mv.toLatin1());

    //脉冲宽度
    QString width = QString("%1").arg(ui->txtWidth->text().toInt(), 4, 10, QChar('0'));
    data.append(width.toLatin1());

    //脉冲周期
    QString cycle = QString("%1").arg(ui->txtCycle->text().toInt(), 4, 10, QChar('0'));
    data.append(cycle.toLatin1());

    //采集点数
    QString point = QString("%1").arg(ui->txtPoint->text().toInt(), 4, 10, QChar('0'));
    data.append(point.toLatin1());

    //结束符号
    data.append(0x03);

    com->write(data);

    qDebug() << TIMEMS << "发送数据:" << QUIHelper::byteArrayToHexStr(data);
}

void frmMain::checkData()
{
    //判断首字节和末尾字节
    int len = buffer.length();
    quint8 head = buffer.at(4);
    quint8 end = buffer.at(len - 1);

    //取出中间数据,转为点数
    if (head == 0x02 && end == 0x03) {
        QByteArray data = buffer.mid(1 + 4, len - 2 - 4);
        int dataLen = len - 2;

        //根据点数自动分页
        int pointCount = dataLen / (1024 * 2);
        pageCurrent = 1;
        pageCount = pointCount / 20;
        if (pointCount % 20 != 0) {
            pageCount++;
        }

        plot_keys.clear();
        plot_values.clear();

        //长度不是偶数,则丢弃最后一个字节
        if (dataLen % 2 != 0) {
            dataLen = dataLen - 1;
        }

        //添加横坐标数据
        for (int i = 0; i < dataLen / 2; i++) {
            plot_keys.append(i);
        }

        //添加纵坐标数据
        for (int i = 0; i < dataLen; i = i + 2) {
            qint16 value = QUIHelper::byteToUShort(data.mid(i, 2));
            plot_values.append(value);
        }

        showData();
    }

    buffer.clear();
    isStart = false;
}

void frmMain::showData()
{
    int count = plot_keys.count();
    plot_keys_current.clear();
    plot_values_current.clear();

    int startIndex = (pageCurrent - 1) * 1024 * 20;
    int endIndex = pageCurrent * 1024 * 20;
    if (endIndex > count) {
        endIndex = count;
    }

    for (int i = startIndex; i < endIndex; i++) {
        plot_keys_current.append(plot_keys.at(i));
        plot_values_current.append(plot_values.at(i));
    }

    ui->widgetPlot->graph(0)->setData(plot_keys_current, plot_values_current);
    ui->widgetPlot->graph(0)->rescaleAxes();
    ui->widgetPlot->replot();

    ui->labInfo->setText(QString("共 %1 页  第 %2 页  |  每页显示 %3 个采样点数").arg(pageCount).arg(pageCurrent).arg(20));
}

void frmMain::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        com = new QextSerialPort(QUIConfig::PortName, QextSerialPort::Polling);
        isOpen = com->open(QIODevice::ReadWrite);

        if (isOpen) {
            //清空缓冲区
            com->flush();
            //设置波特率
            com->setBaudRate((BaudRateType)QUIConfig::BaudRate);
            //设置数据位
            com->setDataBits((DataBitsType)8);
            //设置校验位
            com->setParity((ParityType)0);
            //设置停止位
            com->setStopBits((StopBitsType)0);
            com->setFlowControl(FLOW_OFF);
            com->setTimeout(1);
            ui->btnOpen->setText("关闭");
        } else {
            QUIHelper::showMessageBoxError("打开串口失败,请检查串口号是否存在!");
        }
    } else {
        com->close();
        isOpen = false;
        ui->btnOpen->setText("打开");
    }
}

void frmMain::on_btnSend_clicked()
{
    if (!isOpen) {
        return;
    }

    buffer.clear();
    isStart = true;
    ui->widgetPlot->graph(0)->clearData();
    ui->widgetPlot->replot();

    writeData();
}

void frmMain::on_btnSave_clicked()
{
    //每次的数据都已经存在 plot_values 中
    int count = plot_values.count();
    if (count == 0) {
        QUIHelper::showMessageBoxError("当前没有要保存的数据!");
        return;
    }

    QString name = QString("%1/%2.txt").arg(QUIHelper::appPath()).arg(STRDATETIME);
    QFile file(name);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QStringList list;
        foreach (double d, plot_values) {
            list.append(QString::number(d));
        }

        QString str;
        if (QUIConfig::SaveLine) {
            str = list.join(NEWLINE);
        } else {
            str = list.join(" ");
        }

        file.write(str.toLatin1());
        file.close();
        QUIHelper::showMessageBoxInfo("保存数据成功!", 3);
    }
}

void frmMain::on_btnLoad_clicked()
{
    QString filter = "数据文件 (*.txt *.dat)";
    QString fileName = QFileDialog::getOpenFileName(0, "选择文件", QCoreApplication::applicationDirPath(), filter);
    QString data;

    if (fileName != "") {
        QFile file(fileName);
        file.open(QFile::ReadOnly | QIODevice::Text);
        data = file.readAll();
        file.close();

        plot_data = data.split(" ");
        currentCount = plot_data.length();
        loadPlot();
    }
}

void frmMain::on_btnFirst_clicked()
{
    if (pageCount == 0) {
        return;
    }

    if (pageCurrent != 1) {
        pageCurrent = 1;
        showData();
    }
}

void frmMain::on_btnPre_clicked()
{
    if (pageCurrent > 1) {
        pageCurrent--;
        showData();
    }
}

void frmMain::on_btnNext_clicked()
{
    if (pageCurrent < pageCount) {
        pageCurrent++;
        showData();
    }
}

void frmMain::on_btnLast_clicked()
{
    if (pageCount == 0) {
        return;
    }

    if (pageCurrent != pageCount) {
        pageCurrent = pageCount;
        showData();
    }
}

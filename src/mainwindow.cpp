#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QBarSet>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QChartView>
#include <QSerialPortInfo>
#include <array>
#include <cstring>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sThread(new QThread(this))
    , sWorker(new SerialWorker)
    , portCmbBTimer(new QTimer(this))
    , barSeries(new QBarSeries(this))
    , chart(new QChart(nullptr))
{
    ui->setupUi(this);

    ui->dataWindowScrollBar->setMaximum(FLOAT_PAYLOAD_SIZE - DATA_WINDOW_SIZE);
    connect(ui->dataWindowScrollBar, &QScrollBar::valueChanged, this, &MainWindow::moveDataWindow);

    renderTimer.start();

    sWorker->moveToThread(sThread);
    portCmbBTimer->setSingleShot(true);
    connect(portCmbBTimer, &QTimer::timeout, this, &MainWindow::refreshPortList);

    configChart();
    configChartView();
    refreshPortList();

    connect(this, &MainWindow::connectToPort, sWorker, &SerialWorker::doConnect);
    connect(ui->conBtn, &QPushButton::clicked, [this](){ emit connectToPort(this->ui->portCmbBox->currentText());});
    connect(ui->disconBtn, &QPushButton::clicked, sWorker, &SerialWorker::doDisconnect);

    connect(sWorker, &SerialWorker::crcError, this, &MainWindow::onCrcError);
    connect(sWorker, &SerialWorker::portError, this, &MainWindow::onPortError);
    connect(sWorker, &SerialWorker::packetParsed, this, &MainWindow::onDataReceived);

    connect(sThread, &QThread::finished, sWorker, &QObject::deleteLater);

    sThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;

    if (sThread->isRunning()) {
        sThread->quit();
        sThread->wait(1000);
    }
}

void MainWindow::onDataReceived(const QByteArray &barr_payload)
{
    if(renderTimer.elapsed() < FRAME_UPDATE_TIMEOUT)
    {
        return;
    }
    renderTimer.restart();

    const char* rawData = barr_payload.constData();

    std::array<float, FLOAT_PAYLOAD_SIZE> data;

    for(quint16 i = 0; i < FLOAT_PAYLOAD_SIZE; i++)
    {
        float value;
        memcpy(&value, &rawData[i * sizeof(float)], sizeof(float));
        data[i] = value;
    }

    setupGraph(data);
}

void MainWindow::onCrcError()
{
    qDebug() << "CRC Error reported to GUI";
}

void MainWindow::onPortError(const QString &error)
{
    QMessageBox::critical(this, "Serial Port Error", error);
}

void MainWindow::setupGraph(const std::array<float,FLOAT_PAYLOAD_SIZE> &data)
{
    QBarSet *set = barSeries->barSets().at(0);
    for(int i = 0; i < DATA_WINDOW_SIZE; i++)
    {
        quint16 elem_index = i + dataWindowStart;
        set->replace(i, data[elem_index]);
    }

    if(dataWindowMoved)
    {
        auto *axisX = qobject_cast<QBarCategoryAxis*>(chart->axes(Qt::Horizontal).at(0));
        axisX->setRange(QString::number(dataWindowStart), QString::number(dataWindowStart + DATA_WINDOW_SIZE));
        dataWindowMoved = false;
    }
}

void MainWindow::configChart()
{
    chart->setTheme(QChart::ChartThemeDark);

    QBarSet *set = new QBarSet("Magnitudes");
    for(int i = dataWindowStart; i < dataWindowStart + DATA_WINDOW_SIZE; i++)
    {
        *set << 1.0;
    }
    barSeries->append(set);
    chart->addSeries(barSeries);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 3.3);

    chart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QStringList categories;
    for(int i = 0; i < FLOAT_PAYLOAD_SIZE; i++) { categories << QString::number(i); }
    axisX->append(categories);
    axisX->setRange(QString::number(dataWindowStart), QString::number(dataWindowStart + DATA_WINDOW_SIZE));

    chart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

}

void MainWindow::configChartView()
{
    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::refreshPortList()
{
    const auto serialPortInfos = QSerialPortInfo::availablePorts();

    if(serialPortInfos.count() == ui->portCmbBox->count())
    {
        return;
    }

    QString currentSelection = ui->portCmbBox->currentText();
    ui->portCmbBox->clear();

    for (const QSerialPortInfo &portInfo : serialPortInfos)
    {
        ui->portCmbBox->addItem(portInfo.portName());
    }

    int index = ui->portCmbBox->findText(currentSelection);
    if (index != -1) {
        ui->portCmbBox->setCurrentIndex(index);
    }

    portCmbBTimer->start(PORT_CMBB_UPD_TIMEOUT);
}

void MainWindow::moveDataWindow(int value)
{
    dataWindowStart = value;
    dataWindowMoved = true;
}

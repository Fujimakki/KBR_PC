#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QDialog>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <cstring>
#include <qobject.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sThread(new QThread(this))
    , sWorker(new SerialWorker(this))
    , portCmbBTimer(new QTimer(this))
    , channel0(new WaveformSeries(nullptr))
    , channel1(new WaveformSeries(nullptr))
    , fftData0(new SpectrumSeries(nullptr))
    , fftData1(new SpectrumSeries(nullptr))
{
    ui->setupUi(this);

#ifdef FPS_LOCK
    rawRenderTimer.start();
    fftRenderTimer.start();
#endif // FPS_LOCK

    sWorker->moveToThread(sThread);
    connect(sThread, &QThread::finished, sWorker, &QObject::deleteLater);

    connect(portCmbBTimer, &QTimer::timeout, this, &MainWindow::checkPorts);
    portCmbBTimer->start(PORT_CMBB_UPD_TIMEOUT);
    connect(this, &MainWindow::portsChanged, this, &MainWindow::refreshPortList);

    connect(this, &MainWindow::connectToPort, sWorker, &SerialWorker::doConnect);
    connect(
        ui->conBtn,
        &QPushButton::clicked,
        [this]() {
            emit connectToPort(this->ui->portCmbBox->currentText());
        }
    );
    connect(this, &MainWindow::sendMessage, sWorker, &SerialWorker::sendMessage);
    connect(
        ui->awsBtn,
        &QPushButton::pressed,
        [this]() {
            quint16 awsData = ui->awsSpinBox->value();
            emit sendMessage(TxPacket::AWS, awsData);
        }
    );
    connect(ui->disconBtn, &QPushButton::clicked, sWorker, &SerialWorker::doDisconnect);

    connect(sWorker, &SerialWorker::crcError, this, &MainWindow::onCrcError);
    connect(sWorker, &SerialWorker::portError, this, &MainWindow::onPortError);

    connect(sWorker, &SerialWorker::awsDataParsed, this, &MainWindow::awsDataReceived);
    connect(sWorker, &SerialWorker::rawDataParsed, this, &MainWindow::rawDataReceived);
    connect(sWorker, &SerialWorker::fftDataParsed, this, &MainWindow::fftDataReceived);

    fftData0->setAxesMax(QPointF(1200, 4.4));
    fftData1->setAxesMax(QPointF(1200, 4.4));
    fftData1->setPen(QPen(Qt::red, 1));

    ui->spectrumGraph->setMargins(marginsGraph);
    ui->spectrumGraph->addSeries(fftData0);
    ui->spectrumGraph->addSeries(fftData1);

    channel0->setAxesMax(QPointF(4096, 4.4));
    channel1->setAxesMax(QPointF(4096, 4.4));
    channel1->setPen(QPen(Qt::red));

    ui->waveformGraph->setMargins(marginsGraph);
    ui->waveformGraph->addSeries(channel0);
    ui->waveformGraph->addSeries(channel1);

    checkPorts();
    sThread->start();
}

MainWindow::~MainWindow() {
   delete ui;

    if (sThread->isRunning()) {
        sThread->quit();
        sThread->wait(1000);
    }
}

void MainWindow::awsDataReceived(const QByteArray &barr_payload) {

    QMessageBox::information(this, "AWS", "Averaging window size is successfully set");
    qDebug() << "Averaging window size is successfully set.";
}

void MainWindow::fftDataReceived(const QByteArray &payload) {
#ifdef FPS_LOCK
    if (fftRenderTimer.elapsed() < FRAME_UPDATE_TIMEOUT) {
        return;
    }
    fftRenderTimer.restart();
#endif // FPS_LOCK

    const qsizetype pldByteHalfSize = payload.size() >> 1;
    const qsizetype pldFHalfSize = pldByteHalfSize >> 2;
    QList<float>val0(pldFHalfSize);
    QList<float>val1(pldFHalfSize);

    memcpy(val0.data(), payload.constData(), pldByteHalfSize);
    memcpy(val1.data(), payload.constData() + pldByteHalfSize, pldByteHalfSize);

    fftData0->setValues(val0);
    fftData1->setValues(val1);
}

void MainWindow::rawDataReceived(const QByteArray &payload) {
#ifdef FPS_LOCK
    if (rawRenderTimer.elapsed() < FRAME_UPDATE_TIMEOUT) {
        return;
    }
    rawRenderTimer.restart();
#endif // FPS_LOCK

    QDataStream pldStream(payload);
    pldStream.setByteOrder(QDataStream::LittleEndian);

    const qsizetype pldU16HalfSize = payload.size() >> 2;

    QList<float>val0(pldU16HalfSize);
    QList<float>val1(pldU16HalfSize);

    for (qsizetype i = 0; i < pldU16HalfSize; i++)
    {
        quint32 cpyBuf;
        pldStream >> cpyBuf;

        val0[i] = static_cast<float>(cpyBuf & 0xFFFF) * ADC_QUANT_STEP_V;
        val1[i] = static_cast<float>(cpyBuf >> 16) * ADC_QUANT_STEP_V;
    }

    channel0->setValues(val0);
    channel1->setValues(val1);
}

void MainWindow::onCrcError(const QString &type) { qDebug() << type << ":CRC Error reported to GUI"; }

void MainWindow::onPortError(const QString &error) {
    QMessageBox::critical(this, "Serial Port Error", error);
}

void MainWindow::checkPorts() {
    const auto newSerialPortInfos = QSerialPortInfo::availablePorts();
    static QList<QSerialPortInfo> lastSerialPortInfos;

    if (newSerialPortInfos.size() != lastSerialPortInfos.size()) {
        lastSerialPortInfos = newSerialPortInfos;
        emit portsChanged(lastSerialPortInfos);
        return;
    }

    for (int i = 0; i < newSerialPortInfos.size(); i++)
    {
        if (lastSerialPortInfos[i].portName() != newSerialPortInfos[i].portName())
        {
            lastSerialPortInfos = newSerialPortInfos;
            emit portsChanged(lastSerialPortInfos);
            return;
        }
    }
}

void MainWindow::refreshPortList(const QList<QSerialPortInfo> &ports) {
    ui->portCmbBox->clear();

    for (const auto &port : ports)
    {
        ui->portCmbBox->addItem(port.portName());
    }
}

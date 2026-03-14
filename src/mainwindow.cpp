#include "mainwindow.h"

#include "serialworker.h"
#include "txpacket.h"
#include "ui_mainwindow.h"

#include <QDialog>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <cstdint>
#include <cstring>
#include <qtypes.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    sThread(new QThread(this)),
    sWorker(new SerialWorker(this)),
    portCmbBTimer(new QTimer(this))
{
    ui->setupUi(this);

    channel1.resize(RAW_PAYLOAD_U16);
    channel2.resize(RAW_PAYLOAD_U16);
    fftData.resize(FFT_PAYLOAD_FLOATS);

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

    ui->spectrumGraph->setMaxAxes(QPair<qreal, qreal>(2400, 4.4));
    ui->waveformGraph->setMaxAxes(QPair<qreal, qreal>(4096, 4.4));

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
    // memcpy(&awsData, barr_payload.constData(), sizeof(quint16));

    QMessageBox::information(this, "AWS", "Averaging window size is successfully set");
    qDebug() << "Averaging window size is successfully set.";
}

void MainWindow::fftDataReceived(const QByteArray &barr_payload) {
#ifdef FPS_LOCK
    if (fftRenderTimer.elapsed() < FRAME_UPDATE_TIMEOUT) {
        return;
    }
    fftRenderTimer.restart();
#endif // FPS_LOCK

    const char *payload = barr_payload.constData();

    for (quint16 i = 0; i < FFT_PAYLOAD_FLOATS; i++) {
        float value;
        memcpy(&value, &payload[i * sizeof(float)], sizeof(float));
        fftData[i] = value;
    }

    ui->spectrumGraph->setAmplitudes(fftData);
}

void MainWindow::rawDataReceived(const QByteArray &barr_payload) {
#ifdef FPS_LOCK
    if (rawRenderTimer.elapsed() < FRAME_UPDATE_TIMEOUT) {
        return;
    }
    rawRenderTimer.restart();
#endif // FPS_LOCK

    const char *payload = barr_payload.data();

    for (quint16 i = 0; i < RAW_PAYLOAD_U16; i++)
    {
        uint16_t value;

        memcpy(&value, &payload[(i << 1) * sizeof(value)], sizeof(value));
        channel1[i] = static_cast<qreal>(value) / ((1 << 12) - 1) * 3.3;

        memcpy(&value, &payload[((i << 1) + 1) * sizeof(value)], sizeof(value));
        channel2[i] = static_cast<qreal>(value) / ((1 << 12) - 1) * 3.3;
    }

    ui->waveformGraph->addPoints(channel1);
}

void MainWindow::onCrcError() { qDebug() << "CRC Error reported to GUI"; }

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

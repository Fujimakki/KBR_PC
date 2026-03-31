#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QDialog>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <cstdint>
#include <cstring>
#include <qmargins.h>
#include <qnamespace.h>
#include <qpoint.h>
#include <qtypes.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sThread(new QThread(this))
    , sWorker(new SerialWorker(this))
    , portCmbBTimer(new QTimer(this))
    , channel0(new WaveformSeries(nullptr))
    , channel1(new WaveformSeries(nullptr))
    , fftData(new SpectrumSeries(nullptr))
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

    fftData->setAxesMax(QPointF(1200, 4.4));
    ui->spectrumGraph->setMargins(marginsGraph);
    ui->spectrumGraph->addSeries(fftData);

    channel0->setAxesMax(QPointF(4096, 4.4));
    channel1->setAxesMax(QPointF(4096, 4.4));
    channel1->setPen(QPen(Qt::red));

    ui->waveformGraph->setMargins(marginsGraph);
    ui->waveformGraph->addSeries(channel0);
    ui->waveformGraph->addSeries(channel1);

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

void MainWindow::fftDataReceived(const QByteArray &barr_payload) {
#ifdef FPS_LOCK
    if (fftRenderTimer.elapsed() < FRAME_UPDATE_TIMEOUT) {
        return;
    }
    fftRenderTimer.restart();
#endif // FPS_LOCK

    const char *payload = barr_payload.constData();
    QList<qreal>values(FFT_PAYLOAD_FLOATS);

    for (quint16 i = 0; i < FFT_PAYLOAD_FLOATS; i++)
    {
        float val;
        memcpy(&val, &payload[i * sizeof(val)], sizeof(val));
        values[i] = val;
    }

    fftData->setValues(values);
}

void MainWindow::rawDataReceived(const QByteArray &barr_payload) {
#ifdef FPS_LOCK
    if (rawRenderTimer.elapsed() < FRAME_UPDATE_TIMEOUT) {
        return;
    }
    rawRenderTimer.restart();
#endif // FPS_LOCK

    const char *payload = barr_payload.data();
    QList<qreal>val0(RAW_PAYLOAD_U16);
    QList<qreal>val1(RAW_PAYLOAD_U16);

    for (quint16 i = 0; i < RAW_PAYLOAD_U16; i++)
    {
        uint16_t value;

        memcpy(&value, &payload[(i << 1) * sizeof(value)], sizeof(value));
        val0[i] = static_cast<qreal>(value) / ((1 << 12) - 1) * 3.3;

        memcpy(&value, &payload[((i << 1) + 1) * sizeof(value)], sizeof(value));
        val1[i] = static_cast<qreal>(value) / ((1 << 12) - 1) * 3.3;
    }

    channel0->setValues(val0);
    channel1->setValues(val1);
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

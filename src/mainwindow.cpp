#include "mainwindow.h"
#include "serialworker.h"
#include "txpacket.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QDialog>
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sThread(new QThread(this))
    , sWorker(new SerialWorker)
    , portCmbBTimer(new QTimer(this))
{
    ui->setupUi(this);

    rawData.resize(RAW_PAYLOAD_FLOATS);
    fftData.resize(FFT_PAYLOAD_FLOATS);

#ifdef FPS_LOCK
    rawRenderTimer.start();
    fftRenderTimer.start();
#endif // FPS_LOCK

    sWorker->moveToThread(sThread);
    connect(portCmbBTimer, &QTimer::timeout, this, &MainWindow::checkPorts);
    portCmbBTimer->start(PORT_CMBB_UPD_TIMEOUT);
    connect(this, &MainWindow::portsChanged, this, &MainWindow::refreshPortList);

    ui->spectrumGraph->setSampleRate(ADC_SAMPLE_RATE_HZ);

    connect(this, &MainWindow::connectToPort, sWorker, &SerialWorker::doConnect);
    connect(ui->conBtn, &QPushButton::clicked, [this](){ emit connectToPort(this->ui->portCmbBox->currentText());});
    connect(ui->disconBtn, &QPushButton::clicked, sWorker, &SerialWorker::doDisconnect);

    connect(ui->awsBtn, &QPushButton::pressed, [this](){ emit sendMessage(TxPacket::AWS, QByteArray::number(ui->awsSpinBox->value())); });
    connect(this, &MainWindow::sendMessage, sWorker, &SerialWorker::sendMessage);

    connect(sWorker, &SerialWorker::crcError, this, &MainWindow::onCrcError);
    connect(sWorker, &SerialWorker::portError, this, &MainWindow::onPortError);

    connect(sWorker, &SerialWorker::awsDataParsed, this, &MainWindow::awsDataReceived);
    connect(sWorker, &SerialWorker::rawDataParsed, this, &MainWindow::rawDataReceived);
    connect(sWorker, &SerialWorker::fftDataParsed, this, &MainWindow::fftDataReceived);

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

void MainWindow::awsDataReceived(const QByteArray &barr_payload)
{
    memcpy(&awsData, barr_payload.constData(), sizeof(quint16));
    setupGraph(PayloadType::AWS);
}

void MainWindow::fftDataReceived(const QByteArray &barr_payload)
{
#ifdef FPS_LOCK
    if(fftRenderTimer.elapsed() < FRAME_UPDATE_TIMEOUT)
    {
        return;
    }
    fftRenderTimer.restart();
#endif // FPS_LOCK

    const char* payload = barr_payload.constData();

    for(quint16 i = 0; i < FFT_PAYLOAD_FLOATS; i++)
    {
        float value;
        memcpy(&value, &payload[i * sizeof(float)], sizeof(float));
        fftData[i] = value;
    }

    setupGraph(FFT);
}

void MainWindow::rawDataReceived(const QByteArray &barr_payload)
{
#ifdef FPS_LOCK
    if(rawRenderTimer.elapsed() < FRAME_UPDATE_TIMEOUT)
    {
        return;
    }
    rawRenderTimer.restart();
#endif // FPS_LOCK

    const char* payload = barr_payload.constData();

    for(quint16 i = 0; i < RAW_PAYLOAD_FLOATS; i++)
    {
        float value;
        memcpy(&value, &payload[i * sizeof(float)], sizeof(float));
        rawData[i] = value;
    }

    setupGraph(PayloadType::RAW);
}

void MainWindow::onCrcError()
{
    qDebug() << "CRC Error reported to GUI";
}

void MainWindow::onPortError(const QString &error)
{
    QMessageBox::critical(this, "Serial Port Error", error);
}

void MainWindow::setupGraph(PayloadType type)
{
    switch(type)
    {
        case PayloadType::AWS:
        {
            QMessageBox::information(this, "AWS" , "Averaging window size is successfully set");
            qDebug() << "Averaging window size is successfully set.";
            break;
        }
        case PayloadType::RAW:
        {
            ui->waveformGraph->addPoints(rawData);
            qDebug() << "waveformGraph is updated.";
            break;
        }
        case PayloadType::FFT:
        {
            qDebug() << "spectrumGraph is updated.";
            ui->spectrumGraph->setAmplitudes(fftData);
            break;
        }
        default:
        {
            break;
        }
    }
}

void MainWindow::checkPorts()
{
    const auto newSerialPortInfos = QSerialPortInfo::availablePorts();
    static QList<QSerialPortInfo> lastSerialPortInfos;

    if(newSerialPortInfos.size() != lastSerialPortInfos.size())
    {
        lastSerialPortInfos = newSerialPortInfos;
        emit portsChanged(lastSerialPortInfos);
        return;
    }

    for(int i = 0; i < newSerialPortInfos.size(); i++)
    {
        if(lastSerialPortInfos[i].portName() != newSerialPortInfos[i].portName())
        {
            lastSerialPortInfos = newSerialPortInfos;
            emit portsChanged(lastSerialPortInfos);
            return;
        }
    }
}

void MainWindow::refreshPortList(const QList<QSerialPortInfo> &ports)
{
    ui->portCmbBox->clear();

    for(const auto &port : ports)
    {
        ui->portCmbBox->addItem(port.portName());
    }
}


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QBarSet>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QChartView>
#include <QSerialPortInfo>
#include <cstring>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sThread(new QThread(this))
    , sWorker(new SerialWorker)
    , portCmbBTimer(new QTimer(this))
    , waveform(new QLineSeries(this))
    , chart(new QChart(nullptr))
{
    ui->setupUi(this);

    rawData.resize(RAW_PAYLOAD_FLOATS);
    fftData.resize(FFT_PAYLOAD_FLOATS);

#ifdef FPS_LOCK
    rawRenderTimer.start();
    fftRenderTimer.start();
#endif // FPS_LOCK

    sWorker->moveToThread(sThread);
    portCmbBTimer->setSingleShot(true);
    connect(portCmbBTimer, &QTimer::timeout, this, &MainWindow::refreshPortList);

    ui->spectrumGraph->setSampleRate(ADC_SAMPLE_RATE_HZ);
    refreshPortList();

    connect(this, &MainWindow::connectToPort, sWorker, &SerialWorker::doConnect);
    connect(ui->conBtn, &QPushButton::clicked, [this](){ emit connectToPort(this->ui->portCmbBox->currentText());});
    connect(ui->disconBtn, &QPushButton::clicked, sWorker, &SerialWorker::doDisconnect);

    connect(sWorker, &SerialWorker::crcError, this, &MainWindow::onCrcError);
    connect(sWorker, &SerialWorker::portError, this, &MainWindow::onPortError);

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

    setupGraph(PayloadType::Fft);
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

    setupGraph(PayloadType::Raw);
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
        case PayloadType::Raw:
        {
            // TODO: Implement graph update
            break;
        }
        case PayloadType::Fft:
        {
            ui->spectrumGraph->setAmplitudes(fftData);
            break;
        }
        default:
        {
            break;
        }
    }
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

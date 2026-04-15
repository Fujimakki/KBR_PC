#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <qmargins.h>
#include <qtypes.h>

#include "serialworker.h"
#include "waveformseries.h"
#include "spectrumseries.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void connectToPort(const QString &portName);
    void portsChanged(const QList<QSerialPortInfo> &ports);

private slots:
    void fftDataReceived(const QByteArray &barr_payload);
    void rawDataReceived(const QByteArray &barr_payload);
    void onCrcError(const QString &type);
    void onPortError(const QString &error);
    void checkPorts();
    void refreshPortList(const QList<QSerialPortInfo> &ports);

private:
    enum PayloadType
    {
        RAW,
        FFT
    };

    Ui::MainWindow *ui;

    QThread *sThread = nullptr;
    SerialWorker *sWorker = nullptr;

    QTimer *portCmbBTimer = nullptr;

#ifdef FPS_LOCK
    QElapsedTimer rawRenderTimer;
    QElapsedTimer fftRenderTimer;
    static constexpr qsizetype FRAME_UPDATE_TIMEOUT = 33;    // Time in mseconds
#endif // FPS_LOCK

    static constexpr qsizetype PORT_CMBB_UPD_TIMEOUT = 5000;    // Time in mseconds

    static constexpr qreal ADC_SAMPLE_RATE_HZ = 2.4e6;
    const qreal ADC_QUANT_STEP_V = 3.3 / 4095.0;

    quint16 awsData;
    WaveformSeries *channel0 = nullptr;
    WaveformSeries *channel1 = nullptr;
    SpectrumSeries *fftData0 = nullptr;
    SpectrumSeries *fftData1 = nullptr;
    QMargins marginsGraph = QMargins(35,20,20,20);
};
#endif // MAINWINDOW_H

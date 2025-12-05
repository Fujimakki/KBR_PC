#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QLineSeries>
#include <QChart>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>

#include "serialworker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class TestMainWindow;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void connectToPort(const QString &portName);

private slots:
    void fftDataReceived(const QByteArray &barr_payload);
    void rawDataReceived(const QByteArray &barr_payload);
    void onCrcError();
    void onPortError(const QString &error);
    void refreshPortList();

private:
    enum PayloadType
    {
        Raw,
        Fft
    };

    Ui::MainWindow *ui;

    QThread *sThread = nullptr;
    SerialWorker *sWorker = nullptr;

    QTimer *portCmbBTimer = nullptr;

#ifdef FPS_LOCK
    QElapsedTimer rawRenderTimer;
    QElapsedTimer fftRenderTimer;
#endif // FPS_LOCK

    QLineSeries *waveform = nullptr;
    QChart *chart = nullptr;

    static constexpr quint16 PORT_CMBB_UPD_TIMEOUT = 5000;    // Time in mseconds
    static constexpr quint16 FRAME_UPDATE_TIMEOUT = 33;    // Time in mseconds

    static constexpr quint16 RAW_PAYLOAD_FLOATS = 4096;
    static constexpr quint16 FFT_PAYLOAD_FLOATS = 2048;

    static constexpr qreal ADC_SAMPLE_RATE_HZ = 2.4e6;

    std::vector<qreal> rawData;
    std::vector<qreal> fftData;

    void setupGraph(PayloadType type);
};
#endif // MAINWINDOW_H

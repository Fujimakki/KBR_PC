#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QLineSeries>
#include <QChart>
#include <QTimer>
#include <QElapsedTimer>
#include <qlist.h>
#include <qstringview.h>
#include <qtypes.h>
#include <qwidget.h>
#include <vector>

#include "serialworker.h"
#include "txpacket.h"

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
    void portsChanged(const QList<QSerialPortInfo> &ports);
    void sendMessage(const TxPacket::PacketTypes &type, const quint16 &data);

private slots:
    void awsDataReceived(const QByteArray &barr_payload);
    void fftDataReceived(const QByteArray &barr_payload);
    void rawDataReceived(const QByteArray &barr_payload);
    void onCrcError();
    void onPortError(const QString &error);
    void checkPorts();
    void refreshPortList(const QList<QSerialPortInfo> &ports);

private:
    enum PayloadType
    {
        AWS,
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
#endif // FPS_LOCK

    static constexpr qsizetype PORT_CMBB_UPD_TIMEOUT = 5000;    // Time in mseconds
    static constexpr qsizetype FRAME_UPDATE_TIMEOUT = 33;    // Time in mseconds

    static constexpr qsizetype RAW_PAYLOAD_U16 = 4096;
    static constexpr qsizetype FFT_PAYLOAD_FLOATS = 2048;

    static constexpr qreal ADC_SAMPLE_RATE_HZ = 2.4e6;

    quint16 awsData;
    std::vector<qreal> channel1;
    std::vector<qreal> channel2;
    std::vector<qreal> fftData;
};
#endif // MAINWINDOW_H

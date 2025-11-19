#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QBarSeries>
#include <QChart>
#include <QTimer>
#include <QElapsedTimer>

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
    void onDataReceived(const QByteArray &payload);
    void onCrcError();
    void onPortError(const QString &error);
    void refreshPortList();
    void moveDataWindow(int value);

private:
    Ui::MainWindow *ui;

    QThread *sThread = nullptr;
    SerialWorker *sWorker = nullptr;

    QTimer *portCmbBTimer = nullptr;
    QElapsedTimer renderTimer;

    QBarSeries *barSeries = nullptr;
    QChart *chart = nullptr;

    quint16 dataWindowStart = 0;
    bool dataWindowMoved = false;

    static constexpr quint16 PORT_CMBB_UPD_TIMEOUT = 5000;    // Time in mseconds
    static constexpr quint16 FRAME_UPDATE_TIMEOUT = 33;    // Time in mseconds
    static constexpr quint16 FLOAT_PAYLOAD_SIZE = 512;
    static constexpr quint8 DATA_WINDOW_SIZE = 16;
    static constexpr qreal ADC_SAMPLE_RATE_HZ = 96153.846;

    void setupGraph(const std::array<float,FLOAT_PAYLOAD_SIZE> &data);
    void configChart();
    void configChartView();
};
#endif // MAINWINDOW_H

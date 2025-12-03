#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialWorker : public QObject
{
    Q_OBJECT

    friend class TestSerialWorker;

public:
    explicit SerialWorker(QObject *parent = nullptr);

signals:
    void crcError();
    void portError(const QString &error);

    void rawDataParsed(const QByteArray &payload);
    void fftDataParsed(const QByteArray &payload);

public slots:
    void doConnect(const QString &portName);
    void doDisconnect();

private slots:
    void onReadyRead();
    void handleSerialError(QSerialPort::SerialPortError error);

private:
    QSerialPort *port = nullptr;
    QByteArray rxBuffer;

    static constexpr quint32 PORT_BAUD_RATE = 2250000;    // The value is set on NUCLEO-F446RE
    static constexpr quint16 PORT_BUF_SIZE = 65535;

    static constexpr quint8 PACKET_HEADER_BYTES = 2;
    static constexpr quint8 PACKET_CRC_BYTES = 4;

    static constexpr quint16 RX_RAW_PAYLOAD_BYTES = 16384;
    static constexpr quint16 RX_RAW_PACKET_BYTES = 16390;

    static constexpr quint16 RX_FFT_PAYLOAD_BYTES = 8192;
    static constexpr quint16 RX_FFT_PACKET_BYTES = 8198;

    const QByteArray FFT_PACKET_HEADER = QByteArray::fromHex("AA52");
    const QByteArray RAW_PACKET_HEADER = QByteArray::fromHex("AA51");
    const QByteArray AWS_PACKET_HEADER = QByteArray::fromHex("AA31");

    void processBuffer();
    quint32 parseCrc(const QByteArray &crc);
    quint32 calcCrc(const QByteArray &payload);

};

#endif // SERIALWORKER_H

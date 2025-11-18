#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit SerialWorker(QObject *parent = nullptr);

signals:
    void packetParsed(const QByteArray &payload);
    void crcError();
    void portError(const QString &error);

public slots:
    void doConnect(const QString &portName);
    void doDisconnect();

private slots:
    void onReadyRead();
    void handleSerialError(QSerialPort::SerialPortError error);

private:
    QSerialPort *port;
    QByteArray rxBuffer;

    static constexpr quint32 PORT_BAUD_RATE = 2250000;    // The value is configured for NUCLEO-F446RE
    static constexpr quint16 PORT_BUF_SIZE = 8192;    // If 8192 bytes is not enough, then 65536 bytes may be used

    static constexpr quint16 RX_PACKET_SIZE = 2054;
    const QByteArray PACKET_HEADER = {0xAA, 0x55};
    static constexpr quint8 PACKET_CRC_SIZE = 4;
    static constexpr quint16 RX_PAYLOAD_SIZE = RX_PACKET_SIZE - PACKET_CRC_SIZE - PACKET_HEADER.size();

    void processBuffer();
    quint32 parseCrc(const QByteArray &crc);
    quint32 calcCrc(const QByteArray &payload);

};

#endif // SERIALWORKER_H

#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <qstringview.h>

#include "rxpacket.h"

class SerialWorker : public QObject
{
    Q_OBJECT

    friend class TestSerialWorker;

public:
    explicit SerialWorker(QObject *parent = nullptr);

signals:
    void crcError();
    void typeError(const RxPacket::PacketTypes);
    void portError(const QString &error);

    void awsDataParsed(const QByteArray &payload);
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


    const QByteArray AWS_PACKET_HEADER = QByteArray::fromHex("AA31");

    const QByteArray RAW_PACKET_HEADER = QByteArray::fromHex("AA51");
    const QByteArray FFT_PACKET_HEADER = QByteArray::fromHex("AA52");

    void processBuffer();
};

#endif // SERIALWORKER_H

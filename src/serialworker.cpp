#include "serialworker.h"
#include <zlib.h>
#include <QDebug>
#include <QThread>

SerialWorker::SerialWorker(QObject *parent)
    : QObject{parent}
{

}

void SerialWorker::doConnect(const QString &portName)
{
    if(port)
    {
        port->close();
        delete port;
    }
    port = new QSerialPort(this);

    port->setPortName(portName);    // Sets the name of QSerialPort
    port->setBaudRate(PORT_BAUD_RATE);    // Sets the baud rate of QSerialPort
    port->setReadBufferSize(PORT_BUF_SIZE);    // Sets the size of QSerialPort's internal read buffer

    port->setDataBits(QSerialPort::Data8);    // Sets the count of data bits in a frame
    port->setStopBits(QSerialPort::OneStop);    // Sets the number of stop bits in a frame

    port->setParity(QSerialPort::NoParity);    // Sets the parity checking mode
    port->setFlowControl(QSerialPort::NoFlowControl);    // Sets the desired flow control mode

    connect(port, &QSerialPort::readyRead, this, &SerialWorker::onReadyRead);
    connect(port, &QSerialPort::errorOccurred, this, &SerialWorker::handleSerialError);

    if(port->open(QIODevice::ReadOnly))
    {
        qDebug() << "Serial port opened in thread:" << QThread::currentThread();
    }
    else
    {
        emit portError(port->errorString());
    }
}

void SerialWorker::doDisconnect()
{
    if(port)
    {
        port->close();
        port->deleteLater();
        port = nullptr;
    }
}

void SerialWorker::onReadyRead()
{
    rxBuffer.append(port->readAll());
    processBuffer();
}

void SerialWorker::handleSerialError(QSerialPort::SerialPortError error)
{
    if(error != QSerialPort::NoError) {
        emit portError(port->errorString());
    }
}

void SerialWorker::processBuffer()
{
    while(true)
    {
        auto rawIndex = rxBuffer.indexOf(RAW_PACKET_HEADER);
        auto fftIndex = rxBuffer.indexOf(FFT_PACKET_HEADER);
        if(rawIndex == -1 && fftIndex == -1)
        {
            rxBuffer.clear();
            return;
        }

        qsizetype startIndex = -1;
        bool isRaw = false;

        if( rawIndex != -1 && (fftIndex == -1 || rawIndex < fftIndex) )
        {
            startIndex = rawIndex;
            isRaw = true;
        }
        else if( fftIndex != -1 && (rawIndex == -1 || fftIndex < rawIndex) )
        {
            startIndex = fftIndex;
        }

        if(startIndex > 0)
        {
            rxBuffer.remove(0, startIndex);
        }

        quint16 packetSize = isRaw ? RX_RAW_PACKET_BYTES : RX_FFT_PACKET_BYTES;

        if (rxBuffer.size() < packetSize) {
            return;
        }

        static QByteArray payload = rxBuffer.mid(PACKET_HEADER_BYTES, RX_FFT_PAYLOAD_BYTES);
        QByteArray crc = rxBuffer.right(PACKET_CRC_BYTES);
        rxBuffer.remove(0, RX_FFT_PACKET_BYTES);

        if(parseCrc(crc) == calcCrc(payload))
        {
            if(isRaw)
            {
                emit rawDataParsed(payload);
            }
            else
            {
                emit fftDataParsed(payload);
            }
        }
        else
        {
            emit crcError();
        }
    }
}

quint32 SerialWorker::parseCrc(const QByteArray &crc)
{
    quint32 crcValue = 0;
    for(quint8 i = 0; i < PACKET_CRC_BYTES; i++)
    {
        crcValue |= (static_cast<quint32>(static_cast<quint8>(crc[i]))) << (i << 3);
    }

    return crcValue;
}

quint32 SerialWorker::calcCrc(const QByteArray &payload)
{
    uLong crcValue = crc32(0L, Z_NULL, 0); // Initialize CRC

    // Calculate
    crcValue = crc32(crcValue, reinterpret_cast<const Bytef*>(payload.constData()), payload.size());

    return static_cast<quint32>(crcValue);
}

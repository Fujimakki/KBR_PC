#include "serialworker.h"
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
        port->deleteLater();
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

    if(port->open(QIODevice::ReadWrite))    // Since it is needed to control STM32 port is opened as ReadWrite
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
        RxPacket parser;
        parser.findPacket(rxBuffer);

        if(parser.isPartedPacket())
        {
            return;
        }

        if(parser.isBadPacket())
        {
            emit crcError();
            break;
        }

        switch(parser.getType())
        {
            case RxPacket::AWS:
                emit awsDataParsed(parser.getPayload());
                break;

            case RxPacket::RAW:
                emit rawDataParsed(parser.getPayload());
                break;

            case RxPacket::FFT:
                emit fftDataParsed(parser.getPayload());
                break;

            default:
                emit typeError(parser.getType());
                break;
        }
    }
}


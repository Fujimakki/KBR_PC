#ifndef TXPACKET_H
#define TXPACKET_H

#include <QByteArray>
#include <qcontainerfwd.h>
#include <qstringview.h>

class TxPacket
{
public:
    enum PacketTypes : quint8
    {
        NONE = 0x00,
        AWS = 0x31
    };

    TxPacket() = default;
    ~TxPacket() = default;

    QByteArray operator()(const PacketTypes &type, quint16 payload);

private:
    const QByteArray HEADER_PREFIX = QByteArray::fromHex("AA");

    QByteArray packet;

    QByteArray calcCrc(quint16 data);

};

#endif  // TXPACKET_H

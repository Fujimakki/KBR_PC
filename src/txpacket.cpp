#include "txpacket.h"
#include <qobject.h>
#include <zlib.h>
#include <QIODevice>

QByteArray TxPacket::operator()(const PacketTypes &type, const QByteArray &payload)
{
    if(!packet.isEmpty())
    {
        packet.clear();
    }

    packet = HEADER_PREFIX + QByteArray(1, type) + payload + calcCrc(payload);

    return packet;
}

QByteArray TxPacket::calcCrc(const QByteArray &payload)
{
    uLong crcValue = crc32(0L, Z_NULL, 0); // Initialize CRC

    // Calculate
    crcValue = crc32(crcValue, reinterpret_cast<const Bytef*>(payload.constData()), payload.size());
    crcValue = static_cast<quint32>(crcValue);

    QByteArray crc;
    QDataStream stream(&crc, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint32>(crcValue);

    return crc;
}

#include "txpacket.h"
#include <cstdint>
#include <qobject.h>
#include <qtypes.h>
#include <zlib.h>
#include <QIODevice>

QByteArray TxPacket::operator()(const PacketTypes &type, quint16 pldData)
{
    if(!packet.isEmpty())
    {
        packet.clear();
    }

    char *ptrData = reinterpret_cast<char*>(&pldData);
    QByteArray payload = QByteArray(ptrData, sizeof(quint16));

    packet = HEADER_PREFIX + QByteArray(1, type) + payload + calcCrc(pldData);

    return packet;
}

QByteArray TxPacket::calcCrc(quint16 data)
{
    uint32_t u32data = static_cast<quint32>(data);

    uLong crcValue = crc32(0L, Z_NULL, 0); // Initialize CRC

    // Calculate
    crcValue = crc32(crcValue, reinterpret_cast<const Bytef*>(&u32data), sizeof(quint32));
    crcValue = static_cast<quint32>(crcValue);

    QByteArray crc;
    QDataStream stream(&crc, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint32>(crcValue);

    return crc;
}

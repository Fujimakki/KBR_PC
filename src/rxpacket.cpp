#include "rxpacket.h"
#include <qtypes.h>
#include <utility>
#include <zlib.h>

void RxPacket::findPacket(QByteArray &rxBuffer)
{
    auto header = findHeader(rxBuffer);
    if(header.first == -1 || header.second == PacketTypes::NONE)
    {
        return;
    }

    this->type = header.second;

    quint16 packetSize;
    quint16 payloadSize;
    switch(this->type)
    {
    case RxPacket::AWS:
        packetSize = RX_AWS_PACKET_BYTES;
        payloadSize = RX_AWS_PAYLOAD_BYTES;
        break;

    case RxPacket::RAW:
        packetSize = RX_RAW_PACKET_BYTES;
        payloadSize = RX_RAW_PAYLOAD_BYTES;
        break;

    case RxPacket::FFT:
        packetSize = RX_FFT_PACKET_BYTES;
        payloadSize = RX_FFT_PAYLOAD_BYTES;
        break;

    default:
        break;
    }

    if(header.first > 0)
    {
        rxBuffer.remove(0, header.first);
    }

    if (rxBuffer.size() < packetSize)
    {
        partedPacket = true;
        return;
    }

    payload = rxBuffer.mid(PACKET_HEADER_BYTES, payloadSize);
    crc = rxBuffer.mid(PACKET_HEADER_BYTES + payloadSize, PACKET_CRC_BYTES);
    rxBuffer.remove(0, packetSize);
}

bool RxPacket::checkCrc()
{
    if(payload.isEmpty() || crc.isEmpty())
    {
        return false;
    }
    else
    {
        return calcCrc() == parseCrc();
    }
}

quint32 RxPacket::calcCrc()
{
    uLong crcValue = crc32(0L, Z_NULL, 0); // Initialize CRC

    // Calculate
    crcValue = crc32(crcValue, reinterpret_cast<const Bytef*>(payload.constData()), payload.size());

    return static_cast<quint32>(crcValue);
}

quint32 RxPacket::parseCrc()
{
    quint32 crcValue = 0;
    for(quint8 i = 0; i < PACKET_CRC_BYTES; i++)
    {
        crcValue |= (static_cast<quint32>(static_cast<quint8>(crc[i]))) << (i << 3);
    }

    return crcValue;
}

QPair<qsizetype, RxPacket::PacketTypes> RxPacket::findHeader(const QByteArray &rxBuffer)
{
    QPair<qsizetype, PacketTypes> res;
    res.first = rxBuffer.size();
    res.second = PacketTypes::NONE;

    qsizetype headerIndex;

    for(const auto &header : {PACKET_HEADER_AWS, PACKET_HEADER_RAW, PACKET_HEADER_FFT})
    {
        headerIndex = rxBuffer.indexOf(header);

        if(headerIndex != -1 && headerIndex < res.first)
        {
            res.first = headerIndex;
 
            if(header == PACKET_HEADER_AWS)
            {
                res.second = PacketTypes::AWS;
            }
            else if(type == PACKET_HEADER_RAW)
            {
                res.second = PacketTypes::RAW;
            }
            else if(type == PACKET_HEADER_FFT)
            {
                res.second = PacketTypes::FFT;
            }
            else
            {
                res.second = PacketTypes::NONE;
            }
        }
    }

    return res;
}


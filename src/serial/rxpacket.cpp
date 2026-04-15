#include "rxpacket.h"
#include <zlib.h>

void RxPacket::findPacket(QByteArray &rxBuffer)
{
    auto header = findHeader(rxBuffer);
    if(header.first == -1 || header.second == PacketTypes::NONE)
    {
        partedPacket = true;
        rxBuffer.clear();
        return;
    }
    else
    {
        this->type = header.second;
        rxBuffer.remove(0, header.first);
    }

    quint16 packetSize;
    quint16 payloadSize;
    switch(this->type)
    {
    case RxPacket::RAW:
        packetSize = RAW_PACKET_BYTES;
        payloadSize = RAW_PAYLOAD_BYTES;
        break;

    case RxPacket::FFT:
        packetSize = FFT_PACKET_BYTES;
        payloadSize = FFT_PAYLOAD_BYTES;
        break;

    default:
        break;
    }

    if (rxBuffer.size() < packetSize)
    {
        partedPacket = true;
        return;
    }

    payload = rxBuffer.mid(PACKET_HEADER_BYTES, payloadSize);
    memcpy(&crc, rxBuffer.data() + PACKET_HEADER_BYTES + payloadSize, PACKET_CRC_BYTES);
    rxBuffer.remove(0, packetSize);
}

bool RxPacket::checkCrc()
{
    if(payload.isEmpty())
    {
        return false;
    }
    else
    {
        uint32_t calculatedCrc = calcCrc();
        return calculatedCrc == this->crc;
    }
}

quint32 RxPacket::calcCrc()
{
    uLong crcValue = crc32(0L, Z_NULL, 0); // Initialize CRC

    // Calculate
    crcValue = crc32(crcValue, reinterpret_cast<const Bytef*>(payload.constData()), payload.size());

    return static_cast<quint32>(crcValue);
}

QPair<qsizetype, RxPacket::PacketTypes> RxPacket::findHeader(const QByteArray &rxBuffer)
{
    QPair<qsizetype, PacketTypes> res;
    res.first = rxBuffer.size();
    res.second = PacketTypes::NONE;

    qsizetype headerIndex;

    for(const auto &header : {PACKET_HEADER_RAW, PACKET_HEADER_FFT})
    {
        headerIndex = rxBuffer.indexOf(header);

        if(headerIndex != -1 && headerIndex < res.first)
        {
            res.first = headerIndex;
 
            if(header == PACKET_HEADER_RAW)
            {
                res.second = PacketTypes::RAW;
            }
            else if(header == PACKET_HEADER_FFT)
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


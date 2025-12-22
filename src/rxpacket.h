#ifndef RX_PACKET_H
#define RX_PACKET_H

#include <qcontainerfwd.h>
#include <qtypes.h>
#include <QByteArray>
#include <QString>
#include <QPair>


class RxPacket
{
public:
    enum PacketTypes : quint8
    {
        NONE = 0x00,

        AWS = 0x31,

        RAW = 0x51,
        FFT = 0x52
    };

    static constexpr quint8 PACKET_HEADER_BYTES = 2;
    static constexpr quint8 PACKET_CRC_BYTES = 4;

    static constexpr quint16 RX_AWS_PAYLOAD_BYTES = 2;
    static constexpr quint16 RX_AWS_PACKET_BYTES = 8;

    static constexpr quint16 RX_RAW_PAYLOAD_BYTES = 16384;
    static constexpr quint16 RX_RAW_PACKET_BYTES = 16390;

    static constexpr quint16 RX_FFT_PAYLOAD_BYTES = 8192;
    static constexpr quint16 RX_FFT_PACKET_BYTES = 8198;

    inline RxPacket()
    {
        this->type = PacketTypes::NONE;
        this->partedPacket = false;
    }
    
    ~RxPacket() = default;

    void findPacket(QByteArray &rxBuffer);

    inline bool isBadPacket() { return !checkCrc(); }
    inline bool isPartedPacket() { return this->partedPacket; }

    inline QByteArray getPayload() { return payload; }
    inline PacketTypes getType() { return this->type; }

private:
    PacketTypes type;

    const QByteArray PACKET_HEADER_AWS = QByteArray::fromHex("AA") + QByteArray(1, PacketTypes::AWS);
    const QByteArray PACKET_HEADER_RAW = QByteArray::fromHex("AA") + QByteArray(1, PacketTypes::RAW);
    const QByteArray PACKET_HEADER_FFT = QByteArray::fromHex("AA") + QByteArray(1, PacketTypes::FFT);

    QByteArray crc;
    QByteArray payload;

    bool partedPacket;

    QPair<qsizetype, PacketTypes> findHeader(const QByteArray &rxBuffer);

    quint32 calcCrc();
    quint32 parseCrc();
    bool checkCrc();
};

#endif  // RX_PACKET_H

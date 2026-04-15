#ifndef RX_PACKET_H
#define RX_PACKET_H

#include <QByteArray>
#include <QPair>


class RxPacket
{
public:
    enum PacketTypes : quint8
    {
        NONE = 0x00,

        RAW = 0x51,
        FFT = 0x52
    };

    static constexpr qsizetype PACKET_HEADER_BYTES = 2;
    static constexpr qsizetype PACKET_CRC_BYTES = 4;

    static constexpr qsizetype AWS_PAYLOAD_BYTES = 2;
    const qsizetype AWS_PACKET_BYTES = PACKET_HEADER_BYTES + AWS_PAYLOAD_BYTES + PACKET_CRC_BYTES;

    static constexpr qsizetype RAW_PAYLOAD_BYTES = 16384;
    const qsizetype RAW_PACKET_BYTES = PACKET_HEADER_BYTES + RAW_PAYLOAD_BYTES + PACKET_CRC_BYTES;

    const qsizetype FFT_PAYLOAD_BYTES = ((RAW_PAYLOAD_BYTES >> 2) + 2) << 2;
    const qsizetype FFT_PACKET_BYTES = PACKET_HEADER_BYTES + FFT_PAYLOAD_BYTES + PACKET_CRC_BYTES;

    inline RxPacket()
    {
        this->type = PacketTypes::NONE;
        this->partedPacket = false;
        this->crc = 0;
    }

    ~RxPacket() = default;

    void findPacket(QByteArray &rxBuffer);

    inline bool isBadPacket() { return !checkCrc(); }
    inline bool isPartedPacket() { return this->partedPacket; }

    inline QByteArray getPayload() { return payload; }
    inline PacketTypes getType() const { return this->type; }

private:
    PacketTypes type;

    const QByteArray PACKET_HEADER_RAW = QByteArray::fromHex("AA") + QByteArray(1, PacketTypes::RAW);
    const QByteArray PACKET_HEADER_FFT = QByteArray::fromHex("AA") + QByteArray(1, PacketTypes::FFT);

    uint32_t crc;
    QByteArray payload;

    bool partedPacket;

    QPair<qsizetype, PacketTypes> findHeader(const QByteArray &rxBuffer);

    quint32 calcCrc();
    bool checkCrc();
};

#endif  // RX_PACKET_H

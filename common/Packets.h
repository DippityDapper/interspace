#ifndef SDL3_FIRST_PROJECT_PACKETS_H
#define SDL3_FIRST_PROJECT_PACKETS_H

enum PacketType
{
    PACKET_INPUT,
    PACKET_STATE
};

struct InputPacket
{
    PacketType type = PACKET_INPUT;
    bool up, down, left, right;
};

struct StatePacket
{
    PacketType type = PACKET_STATE;
    int entityId;
    float x, y;
};

#endif

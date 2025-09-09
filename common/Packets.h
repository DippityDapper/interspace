#ifndef SDL3_FIRST_PROJECT_PACKETS_H
#define SDL3_FIRST_PROJECT_PACKETS_H

enum PacketType
{
    PACKET_INPUT,
    PACKET_STATE,
    PACKET_DISCONNECT,
    PACKET_CONNECT,
    PACKET_CLIENT_DATA,
    PACKET_CREATE_CLIENT_ENTITY,
    PACKET_POSITION
};

struct InputPacket
{
    PacketType type = PACKET_INPUT;
    int clientId;
    bool up, down, left, right;
};

struct PositionPacket
{
    PacketType type = PACKET_POSITION;
    int clientId;
    float x;
    float y;
};

struct StatePacket
{
    PacketType type = PACKET_STATE;
    int clientId;
    float x, y;
};

struct ClientDataPacket
{
    PacketType type = PACKET_CONNECT;
    int clientId;
};

struct DisconnectPacket
{
    PacketType type = PACKET_DISCONNECT;
    int clientId;
};

#endif

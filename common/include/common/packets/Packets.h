#pragma once

namespace Engine
{
    enum PacketType
    {
        PACKET_DISCONNECT,
        PACKET_CONNECT,
        PACKET_CLIENT_DATA,
        PACKET_CREATE_CLIENT_ENTITY,
        PACKET_POSITION
    };

    struct PositionPacket
    {
        PacketType type = PACKET_POSITION;
        int clientId = -1;
        float x{};
        float y{};
    };

    struct ClientDataPacket
    {
        PacketType type = PACKET_CONNECT;
        int clientId = -1;
    };

    struct DisconnectPacket
    {
        PacketType type = PACKET_DISCONNECT;
        int clientId = -1;
    };
}

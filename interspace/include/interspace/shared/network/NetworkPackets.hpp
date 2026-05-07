#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace Interspace
{
    enum NetMessageType : uint16_t
    {
        CONNECTION_REQUEST,
        CONNECTION_REQUEST_, // Connection picked up but not sent by the client
        CONNECTION_ACCEPTED,
        CLIENT_CONNECTED,

        DISCONNECTION_REQUEST,
        DISCONNECTION_REQUEST_, // Disconnection picked up but not sent by the client
        DISCONNECTION_ACKNOWLEDGED,
        CLIENT_DISCONNECTED,

        PLAYER_DATA_PACKET,
    };

    struct WorldPacket
    {
        uint16_t x = 0;
        uint16_t y = 0;
        std::string name{};
        uint32_t seed = 0;
    };

    struct TilePacket
    {
        uint32_t id = 0;
        uint32_t variant = 0;
    };

    struct ChunkPacket
    {
        uint16_t x = 0;
        uint16_t y = 0;
        uint64_t timestamp = 0;
        std::vector<TilePacket> tiles{};
    };

    struct ChunkRequestPacket
    {
        uint16_t x = 0;
        uint16_t y = 0;
        uint64_t timestamp = 0;
        uint8_t type = 0;
    };
}

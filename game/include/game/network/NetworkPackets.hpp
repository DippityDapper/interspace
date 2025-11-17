#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace Game
{
    enum NetMessageType : uint8_t
    {
        CONNECTION_REQUEST,
        CONNECTION_REQUEST_, // Connection picked up but not sent by the client
        CONNECTION_ACCEPTED,
        CLIENT_CONNECTED,

        DISCONNECTION_REQUEST,
        DISCONNECTION_REQUEST_, // Disconnection picked up but not sent by the client
        DISCONNECTION_ACKNOWLEDGED,
        CLIENT_DISCONNECTED,

        WORLD_DATA_REQUEST,
        WORLD_DATA_PACKET,

        AREA_DATA_REQUEST,
        AREA_DATA_PACKET,

        FACTION_DATA_REQUEST,
        FACTION_DATA_PACKET,

        POSITION_REQUEST,
        POSITION_PACKET,

        COLONIST_POSITION_REQUEST,
        COLONIST_POSITION_PACKET,
    };

    inline void PackBytes(std::vector<uint8_t>& buffer, const void* data, size_t len)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
        buffer.insert(buffer.end(), bytes, bytes + len);
    }

    inline std::string UnpackString(const std::vector<uint8_t>& data, size_t& offset, uint32_t len)
    {
        std::string str(reinterpret_cast<const char*>(&data[offset]), len);
        offset += len;
        return str;
    }

    inline float UnpackFloat(const std::vector<uint8_t>& data, size_t& offset)
    {
        float num = 0;
        memcpy(&num, &data[offset], sizeof(float));
        offset += sizeof(float);
        return num;
    }

    inline uint8_t UnpackUint8(const std::vector<uint8_t>& data, size_t& offset)
    {
        uint8_t num = 0;
        memcpy(&num, &data[offset], sizeof(uint8_t));
        offset += sizeof(uint8_t);
        return num;
    }

    inline uint16_t UnpackUint16(const std::vector<uint8_t>& data, size_t& offset)
    {
        uint16_t num = 0;
        memcpy(&num, &data[offset], sizeof(uint16_t));
        offset += sizeof(uint16_t);
        return num;
    }

    inline uint32_t UnpackUint32(const std::vector<uint8_t>& data, size_t& offset)
    {
        uint32_t num = 0;
        memcpy(&num, &data[offset], sizeof(uint32_t));
        offset += sizeof(uint32_t);
        return num;
    }

    inline uint64_t UnpackUint64(const std::vector<uint8_t>& data, size_t& offset)
    {
        uint64_t num = 0;
        memcpy(&num, &data[offset], sizeof(uint64_t));
        offset += sizeof(uint64_t);
        return num;
    }
}

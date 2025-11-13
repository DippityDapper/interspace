#pragma once

#include <cstdint>

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
};

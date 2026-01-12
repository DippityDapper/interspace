#pragma once

#include <cstdint>

namespace Interspace
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

        WORLD_DATA_PACKET,
        FACTION_DATA_PACKET,

        CREATE_FACTION_REQUEST,
        CREATE_FACTION_ACCEPTED,
        CREATE_FACTION_DENIED,

        CREATE_COLONIST_REQUEST,

        COLONIST_POSITION_REQUEST,
        COLONIST_POSITION_PACKET,

        COLONIST_SELECT_REQUEST,
        COLONIST_SELECTED_PACKET,

        COLONIST_DESELECT_REQUEST,
        COLONIST_DESELECTED_PACKET,

        COLONIST_DESELECT_ALL_REQUEST,
        COLONIST_DESELECTED_ALL_PACKET,

        CHUNK_GENERATED_PACKET,
        CHUNK_REMOVED_PACKET,
    };
}

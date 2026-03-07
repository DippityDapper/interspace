#include "interspace/client/ClientWorld.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/network/NetworkPackets.hpp"

namespace Interspace::Client
{
    void ClientWorld::RegisterNetEvents()
    {
        client->ConnectToEvent(WORLD_DATA_PACKET, this, &ClientWorld::OnWorldDataReceived);
        client->ConnectToEvent(CHUNK_PACKET, this, &ClientWorld::OnChunkPacketReceived);

        client->ConnectToEvent(FACTION_DATA_PACKET, this, &ClientWorld::OnFactionDataReceived);
        client->ConnectToEvent(CREATE_FACTION_REQUEST, this, &ClientWorld::OnCreateFactionRequestReceived);

        client->ConnectToEvent(COLONIST_POSITION_PACKET, this, &ClientWorld::OnColonistPositionDataReceived);
        client->ConnectToEvent(COLONIST_SELECTED_PACKET, this, &ClientWorld::OnColonistSelectedPacketReceived);
        client->ConnectToEvent(COLONIST_DESELECTED_ALL_PACKET, this, &ClientWorld::OnColonistDeselectedAllDataReceived);
        client->ConnectToEvent(COLONIST_DESELECTED_PACKET, this, &ClientWorld::OnColonistDeselectedDataReceived);

        client->ConnectToEvent(DISCONNECTION_ACKNOWLEDGED, this, &ClientWorld::OnDisconnectAcknowledged);
        client->ConnectToEvent(DISCONNECTION_REQUEST_, this, &ClientWorld::OnDisconnectAcknowledged);
    }
}

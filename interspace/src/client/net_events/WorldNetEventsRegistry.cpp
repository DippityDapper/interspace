#include "interspace/client/World.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/network/NetworkPackets.hpp"

namespace Interspace::Client
{
    void World::RegisterNetEvents()
    {
        client->ConnectToEvent(WORLD_DATA_PACKET, this, &World::OnWorldDataReceived);

        client->ConnectToEvent(FACTION_DATA_PACKET, this, &World::OnFactionDataReceived);

        client->ConnectToEvent(COLONIST_POSITION_PACKET, this, &World::OnColonistPositionDataReceived);
        client->ConnectToEvent(COLONIST_SELECTED_PACKET, this, &World::OnColonistSelectedDataReceived);
        client->ConnectToEvent(COLONIST_DESELECTED_ALL_PACKET, this, &World::OnColonistDeselectedAllDataReceived);
        client->ConnectToEvent(COLONIST_DESELECTED_PACKET, this, &World::OnColonistDeselectedDataReceived);

        client->ConnectToEvent(DISCONNECTION_ACKNOWLEDGED, this, &World::OnDisconnectAcknowledged);
        client->ConnectToEvent(DISCONNECTION_REQUEST_, this, &World::OnDisconnectAcknowledged);
    }
}

#include "interspace/server/World.hpp"
#include "interspace/network/NetworkPackets.hpp"

namespace Interspace::Server
{
    void World::RegisterNetEvents()
    {
        server->ConnectToEvent(CLIENT_CONNECTED, this, &World::OnClientConnected);
        server->ConnectToEvent(CLIENT_DISCONNECTED, this, &World::OnClientDisconnected);

        server->ConnectToEvent(COLONIST_POSITION_REQUEST, this, &World::OnColonistPositionRequestReceived);
        server->ConnectToEvent(COLONIST_SELECT_REQUEST, this, &World::OnColonistSelectRequestReceived);
        server->ConnectToEvent(COLONIST_DESELECT_ALL_REQUEST, this, &World::OnColonistDeselectAllRequestReceived);
        server->ConnectToEvent(COLONIST_DESELECT_REQUEST, this, &World::OnColonistDeselectRequestReceived);
        server->ConnectToEvent(CREATE_COLONIST_REQUEST, this, &World::OnCreateColonistRequestReceived);

        server->ConnectToEvent(CREATE_FACTION_REQUEST, this, &World::OnCreateFactionRequestReceived);
    }
}

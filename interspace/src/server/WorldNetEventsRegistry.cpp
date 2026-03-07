#include "interspace/server/ServerWorld.hpp"
#include "interspace/network/NetworkPackets.hpp"

namespace Interspace::Server
{
    void ServerWorld::RegisterNetEvents()
    {
        server->ConnectToEvent(CLIENT_CONNECTED, this, &ServerWorld::OnClientConnected);
        server->ConnectToEvent(CLIENT_DISCONNECTED, this, &ServerWorld::OnClientDisconnected);

        server->ConnectToEvent(COLONIST_POSITION_REQUEST, this, &ServerWorld::OnColonistPositionRequestReceived);
        server->ConnectToEvent(COLONIST_SELECT_REQUEST, this, &ServerWorld::OnColonistSelectRequestReceived);
        server->ConnectToEvent(COLONIST_DESELECT_ALL_REQUEST, this, &ServerWorld::OnColonistDeselectAllRequestReceived);
        server->ConnectToEvent(COLONIST_DESELECT_REQUEST, this, &ServerWorld::OnColonistDeselectRequestReceived);
        server->ConnectToEvent(CREATE_COLONIST_REQUEST, this, &ServerWorld::OnCreateColonistRequestReceived);

        server->ConnectToEvent(CREATE_FACTION_REQUEST, this, &ServerWorld::OnCreateFactionRequestReceived);

        server->ConnectToEvent(PLAYER_POSITION_PACKET, this, &ServerWorld::OnPlayerPositionReceived);
        server->ConnectToEvent(CHUNK_REQUEST, this, &ServerWorld::OnChunkRequestReceived);
    }
}

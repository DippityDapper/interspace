#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/ClientWorld.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/server/ServerWorld.hpp"

#include <ranges>
#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Client
    {
        void ClientWorld::RequestColonistSelect(faction_id_t factionId, entity_id_t colonistId)
        {
            std::vector<uint8_t> data{COLONIST_SELECT_REQUEST};

            Engine::Serializer serializer(data);
            serializer << factionId << colonistId << client->clientId;

            client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Server
    {
        void ServerWorld::OnColonistSelectRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            faction_id_t factionId = 0;
            entity_id_t colonistId = 0;
            client_id_t clientId = 0;

            Engine::Deserializer deserializer(data);
            deserializer >> factionId >> colonistId >> clientId;

            if (!Game::server->CheckPeer(clientId, from))
                return;

            if (!factions.contains(factionId))
                return;

            ServerFaction* faction = factions[factionId].get();
            if (!faction->members.contains(clientId) || !faction->colonists.contains(colonistId))
                return;

            ServerColonist* colonist = faction->colonists[colonistId].get();
            colonist->selectedBy = clientId;

            BroadcastColonistSelection(factionId, colonistId, clientId);
        }
    }
}
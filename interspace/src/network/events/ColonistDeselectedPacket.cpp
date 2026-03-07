#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/ClientWorld.hpp"
#include "interspace/server/ServerWorld.hpp"

#include <ranges>
#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Server
    {
        void ServerWorld::BroadcastColonistDeselection(faction_id_t factionId, entity_id_t colonistId, client_id_t clientId)
        {
            std::vector<uint8_t> broadcastData{COLONIST_DESELECTED_PACKET};

            Engine::Serializer serializer(broadcastData);
            serializer << factionId << colonistId << clientId;

            for (const auto& peer: server->GetPeers() | std::views::values)
                server->netInterface->SendToClient(peer, broadcastData, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Client
    {
        void ClientWorld::OnColonistDeselectedDataReceived(const std::vector<uint8_t>& data)
        {
            faction_id_t factionId = 0;
            entity_id_t colonistId = 0;
            client_id_t clientId = 0;

            Engine::Deserializer deserializer(data);
            deserializer >> factionId >> colonistId >> clientId;

            if (!factions.contains(factionId))
                return;

            ClientFaction* faction = factions[factionId].get();
            if (!faction->colonists.contains(colonistId))
                return;

            ClientColonist* colonist = faction->colonists[colonistId].get();
            colonist->selectedBy = 0;

            colonist->sprite->SetTexture("assets/colonists/colonist_blue_spritesheet.png");
        }
    }
}
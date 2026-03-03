#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/World.hpp"
#include "interspace/server/World.hpp"

#include <ranges>
#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Server
    {
        void World::BroadcastColonistDeselection(uint16_t factionId, uint32_t colonistId, uint32_t clientId)
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
        void World::OnColonistDeselectedDataReceived(const std::vector<uint8_t>& data)
        {
            uint16_t factionId = 0;
            uint32_t colonistId = 0;
            uint32_t clientId = 0;

            Engine::Deserializer deserializer(data);
            deserializer >> factionId >> colonistId >> clientId;

            if (!factions.contains(factionId))
                return;

            Faction* faction = factions[factionId].get();
            if (!faction->colonists.contains(colonistId))
                return;

            Colonist* colonist = faction->colonists[colonistId].get();
            colonist->colonistData.selectedBy = 0;

            colonist->sprite->SetTexture("assets/colonists/colonist_blue_spritesheet.png");
        }
    }
}
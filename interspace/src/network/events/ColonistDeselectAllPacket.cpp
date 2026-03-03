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
        void World::BroadcastColonistDeselectAllPacket(uint32_t clientId)
        {
            std::vector<std::tuple<uint16_t, uint32_t>> selected{};
            std::vector<uint8_t> broadcastData{COLONIST_DESELECTED_ALL_PACKET};
            Engine::Serializer serializer(broadcastData);

            for (const auto& faction: factions | std::views::values)
            {
                for (const auto& colonist: faction->colonists | std::views::values)
                {
                    if (colonist->colonistData.selectedBy == clientId)
                    {
                        selected.emplace_back(faction->data.id, colonist->entityData.id);
                        colonist->colonistData.selectedBy = 0;
                    }
                }
            }

            uint16_t selectedCount = selected.size();
            if (selectedCount == 0)
                return;

            serializer << selectedCount;
            for (const auto& [factionId, colonistId]: selected)
            {
                serializer << factionId << colonistId;
            }

            for (const auto& peer: server->GetPeers() | std::views::values)
                server->netInterface->SendToClient(peer, broadcastData, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Client
    {
        void World::OnColonistDeselectedAllDataReceived(const std::vector<uint8_t>& data)
        {
            Engine::Deserializer deserializer(data);
            uint16_t selectedCount = 0;
            deserializer >> selectedCount;
            for (int i = 0; i < selectedCount; i++)
            {
                uint16_t factionId = 0;
                uint32_t colonistId = 0;
                deserializer >> factionId >> colonistId;

                if (!factions.contains(factionId))
                    continue;

                Faction* faction = factions[factionId].get();
                if (!faction->colonists.contains(colonistId))
                    continue;

                Colonist* colonist = faction->colonists[colonistId].get();
                colonist->colonistData.selectedBy = 0;
                colonist->sprite->SetTexture("assets/colonists/colonist_blue_spritesheet.png");
            }
        }
    }
}
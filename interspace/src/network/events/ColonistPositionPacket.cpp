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
        void World::BroadcastColonistPositionData()
        {
            std::vector<std::vector<uint8_t>> sendQueue;

            for (const auto& faction: factions | std::views::values)
            {
                for (const auto& colonist: faction->colonists | std::views::values)
                {
                    std::vector<uint8_t> data{COLONIST_POSITION_PACKET};
                    Engine::Serializer serializer(data);

                    serializer
                            << faction->data.id
                            << colonist->entityData.id
                            << colonist->entityData.position.x
                            << colonist->entityData.position.y;

                    sendQueue.push_back(data);
                }
            }

            for (const auto& peer: server->GetPeers() | std::views::values)
            {
                for (const auto& positionData: sendQueue)
                {
                    server->netInterface->SendToClient(peer, positionData, 0);
                }
            }
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Client
    {
        void World::OnColonistPositionDataReceived(const std::vector<uint8_t>& data)
        {
            Engine::Deserializer deserializer(data);

            uint16_t factionId = 0;
            uint32_t colonistId = 0;
            float colonistPositionX{};
            float colonistPositionY{};

            deserializer >> factionId >> colonistId >> colonistPositionX >> colonistPositionY;

            if (!factions.contains(factionId))
                return;

            Faction* faction = factions[factionId].get();
            Colonist* colonist = faction->GetColonist(colonistId);

            if (!colonist)
                return;

            colonist->entityData.position.x = colonistPositionX;
            colonist->entityData.position.y = colonistPositionY;
        }
    }
}
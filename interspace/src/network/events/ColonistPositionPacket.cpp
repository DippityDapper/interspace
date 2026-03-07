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
        void ServerWorld::BroadcastColonistPositionData()
        {
            std::vector<std::vector<uint8_t>> sendQueue;

            for (const auto& faction: factions | std::views::values)
            {
                for (const auto& colonist: faction->colonists | std::views::values)
                {
                    std::vector<uint8_t> data{COLONIST_POSITION_PACKET};
                    Engine::Serializer serializer(data);

                    serializer
                            << faction->id
                            << colonist->id
                            << colonist->position.x
                            << colonist->position.y;

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
        void ClientWorld::OnColonistPositionDataReceived(const std::vector<uint8_t>& data)
        {
            Engine::Deserializer deserializer(data);

            faction_id_t factionId = 0;
            entity_id_t colonistId = 0;
            float colonistPositionX{};
            float colonistPositionY{};

            deserializer >> factionId >> colonistId >> colonistPositionX >> colonistPositionY;

            if (!factions.contains(factionId))
                return;

            ClientFaction* faction = factions[factionId].get();
            ClientColonist* colonist = faction->GetColonist(colonistId);

            if (!colonist)
                return;

            colonist->position.x = colonistPositionX;
            colonist->position.y = colonistPositionY;
        }
    }
}
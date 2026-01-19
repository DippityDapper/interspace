#include <ranges>

#include "interspace/network/NetworkPackets.hpp"
#include "../../../../../igneous/include/igneous/networking/Serializer.hpp"
#include "interspace/server/World.hpp"

namespace Interspace::Server
{
    void World::OnCreateFactionRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        uint32_t playerId = 0;
        std::string factionName{};

        Engine::Deserializer deserializer(data);
        deserializer >> playerId >> factionName;

        uint16_t factionId = AddFaction(factionName, playerId);
        if (factionId == 0)
        {
            std::vector<uint8_t> deniedData{CREATE_FACTION_DENIED};

            std::string errorMessage{"Faction already exists."};
            Engine::Serializer serializer(deniedData);
            serializer << errorMessage;

            server->netInterface->SendToClient(from, deniedData, ENET_PACKET_FLAG_RELIABLE);
        }
        else
        {
            std::string playerName = server->GetUsername(playerId);
            AddColonistToFaction(factionId, playerName);

            std::vector<uint8_t> acceptedData{CREATE_FACTION_ACCEPTED};
            server->netInterface->SendToClient(from, acceptedData, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    void World::SendFactionData(ENetPeer* to)
    {
        std::vector<uint8_t> data{FACTION_DATA_PACKET};
        Engine::Serializer serializer(data);

        int32_t factionsCount = factions.size();
        serializer << factionsCount;

        for (const auto& faction: factions | std::views::values)
        {
            uint32_t membersCount = faction->data.members.size();

            serializer
                    << faction->data.id
                    << faction->data.name
                    << faction->data.ownerId
                    << membersCount;

            for (const auto& memberId: faction->data.members | std::views::keys)
            {
                serializer << memberId;
            }

            uint32_t colonistCount = faction->colonists.size();
            serializer << colonistCount;

            for (const auto& kvp: faction->colonists)
            {
                uint16_t colonistId = kvp.first;
                Colonist* colonist = kvp.second.get();

                serializer
                        << colonistId
                        << colonist->entityData.name
                        << colonist->entityData.position.x
                        << colonist->entityData.position.y
                        << colonist->colonistData.selectedBy;
            }
        }

        server->netInterface->SendToClient(to, data, ENET_PACKET_FLAG_RELIABLE);
    }

    void World::BroadcastFactionData(uint16_t factionId)
    {
        std::vector<uint8_t> data{FACTION_DATA_PACKET};
        Engine::Serializer serializer(data);

        if (!factions.contains(factionId))
            return;

        uint32_t factionsCount = 1;
        serializer << factionsCount;

        Faction* faction = factions[factionId].get();

        uint32_t membersCount = faction->data.members.size();

        serializer
                << faction->data.id
                << faction->data.name
                << faction->data.ownerId
                << membersCount;

        for (const auto& memberId: faction->data.members | std::views::keys)
        {
            serializer << memberId;
        }

        uint32_t colonistCount = faction->colonists.size();
        serializer << colonistCount;

        for (const auto& kvp: faction->colonists)
        {
            uint16_t colonistId = kvp.first;
            Colonist* colonist = kvp.second.get();

            serializer
                    << colonistId
                    << colonist->entityData.name
                    << colonist->entityData.position.x
                    << colonist->entityData.position.y
                    << colonist->colonistData.selectedBy;
        }

        for (const auto& peer: server->GetPeers() | std::views::values)
            server->netInterface->SendToClient(peer, data, ENET_PACKET_FLAG_RELIABLE);
    }
}

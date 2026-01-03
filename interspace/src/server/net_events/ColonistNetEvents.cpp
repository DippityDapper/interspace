#include <ranges>

#include "interspace/network/NetworkPackets.hpp"
#include "interspace/network/Serializer.hpp"
#include "interspace/server/World.hpp"
#include "SDL3/SDL_log.h"

namespace Interspace::Server
{
    void World::OnColonistPositionRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        uint16_t factionId = 0;
        uint16_t colonistId = 0;
        float colonistPositionX = 0;
        float colonistPositionY = 0;

        Deserializer deserializer(data);
        deserializer
            >> factionId
            >> colonistId
            >> colonistPositionX
            >> colonistPositionY;

        if (!factions.contains(factionId))
            return;

        Faction* faction = factions[factionId].get();
        Colonist* colonist = faction->GetColonist(colonistId);

        if (!colonist)
            return;

        colonist->entityData.position.x = colonistPositionX;
        colonist->entityData.position.y = colonistPositionY;
    }

    void World::OnColonistSelectRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        uint16_t factionId = 0;
        uint16_t colonistId = 0;
        uint32_t clientId = 0;

        Deserializer deserializer(data);
        deserializer
            >> factionId
            >> colonistId
            >> clientId;

        if (!factions.contains(factionId))
            return;

        Faction* faction = factions[factionId].get();
        if (!faction->data.members.contains(clientId) || !faction->colonists.contains(colonistId))
            return;

        Colonist* colonist = faction->colonists[colonistId].get();
        colonist->colonistData.selectedBy = clientId;

        std::vector<uint8_t> broadcastData{COLONIST_SELECTED_PACKET};

        Serializer serializer(broadcastData);
        serializer
            << factionId
            << colonistId
            << clientId;

        for (const auto& peer : server->GetPeers() | std::views::values)
            server->netInterface->SendToClient(peer, broadcastData, ENET_PACKET_FLAG_RELIABLE);
    }

    void World::OnColonistDeselectAllRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        uint32_t clientId = 0;

        Deserializer deserializer(data);
        deserializer >> clientId;

        BroadcastColonistDeselectAllData(clientId);
    }

    void World::OnColonistDeselectRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        uint16_t factionId = 0;
        uint16_t colonistId = 0;
        uint32_t clientId = 0;

        Deserializer deserializer(data);
        deserializer
            >> factionId
            >> colonistId
            >> clientId;

        if (!factions.contains(factionId))
            return;

        Faction* faction = factions[factionId].get();
        if (!faction->data.members.contains(clientId) || !faction->colonists.contains(colonistId))
            return;

        Colonist* colonist = faction->colonists[colonistId].get();
        colonist->colonistData.selectedBy = 0;

        std::vector<uint8_t> broadcastData{COLONIST_DESELECTED_PACKET};

        Serializer serializer(broadcastData);
        serializer
            << factionId
            << colonistId
            << clientId;

        for (const auto& peer : server->GetPeers() | std::views::values)
            server->netInterface->SendToClient(peer, broadcastData, ENET_PACKET_FLAG_RELIABLE);
    }

    void World::OnCreateColonistRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        uint16_t factionId = 0;
        std::string colonistName{};

        Deserializer deserializer(data);
        deserializer >> factionId >> colonistName;

        if (!factions.contains(factionId))
            return;

        AddColonistToFaction(factionId, colonistName);
    }

    void World::BroadcastColonistPositionData()
    {
        std::vector<std::vector<uint8_t>> sendQueue;

        for (const auto& faction : factions | std::views::values)
        {
            for (const auto& colonist : faction->colonists | std::views::values)
            {
                std::vector<uint8_t> data{COLONIST_POSITION_PACKET};
                Serializer serializer(data);

                serializer
                    << faction->data.id
                    << colonist->entityData.id
                    << colonist->entityData.position.x
                    << colonist->entityData.position.y;

                sendQueue.push_back(data);
            }
        }

        for (const auto& peer : server->GetPeers() | std::views::values)
        {
            for (const auto& positionData : sendQueue)
            {
                server->netInterface->SendToClient(peer, positionData, 0);
            }
        }
    }

    void World::BroadcastColonistDeselectAllData(uint32_t clientId)
    {
        std::vector<std::tuple<uint16_t, uint16_t>> selected{};
        std::vector<uint8_t> broadcastData{COLONIST_DESELECTED_ALL_PACKET};
        Serializer serializer(broadcastData);

        for (const auto& faction : factions | std::views::values)
        {
            for (const auto& colonist : faction->colonists | std::views::values)
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
        for (const auto& [factionId, colonistId] : selected)
        {
            serializer
                << factionId
                << colonistId;
        }

        for (const auto& peer : server->GetPeers() | std::views::values)
            server->netInterface->SendToClient(peer, broadcastData, ENET_PACKET_FLAG_RELIABLE);
    }
}

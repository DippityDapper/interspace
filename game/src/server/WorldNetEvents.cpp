#include <ranges>

#include "game/server/World.hpp"

#include "game/network/NetworkPackets.hpp"

namespace Game::Server
{
    void World::RegisterNetEvents()
    {
        server->ConnectToEvent(CLIENT_CONNECTED, this, &World::OnClientConnected);
    }

    void World::OnClientConnected(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        size_t offset = 1;

        uint32_t clientId = UnpackUint32(data, offset);
        // uint32_t usernameLength = UnpackUint32(data, offset);
        // std::string username = UnpackString(data, offset, usernameLength);

        SendWorldData(from);
        SendFactionData(from);

        uint16_t newFactionId = AddFaction(clientId);
        BroadcastFactionData(newFactionId);
    }

    void World::SendWorldData(ENetPeer* to)
    {
        std::vector<uint8_t> _worldData{};
        _worldData.push_back(WORLD_DATA_PACKET);

        PackBytes(_worldData, &worldData->worldSizeX, sizeof(worldData->worldSizeX));
        PackBytes(_worldData, &worldData->worldSizeY, sizeof(worldData->worldSizeY));

        uint32_t worldNameLength = worldName.size();
        PackBytes(_worldData, &worldNameLength, sizeof(worldNameLength));
        PackBytes(_worldData, worldName.data(), worldNameLength);

        server->netInterface->SendToClient(to, _worldData, ENET_PACKET_FLAG_RELIABLE);
    }

    void World::SendFactionData(ENetPeer* to)
    {
        std::vector<uint8_t> factionData{};
        factionData.push_back(FACTION_DATA_PACKET);

        uint32_t factionsCount = factions.size();
        PackBytes(factionData, &factionsCount, sizeof(factionsCount));

        for (const auto& faction : factions | std::views::values)
        {
            PackBytes(factionData, &faction->data.id, sizeof(faction->data.id));

            uint32_t factionNameLength = faction->data.name.size();
            PackBytes(factionData, &factionNameLength, sizeof(factionNameLength));
            PackBytes(factionData, faction->data.name.data(), factionNameLength);

            PackBytes(factionData, &faction->data.ownerId, sizeof(faction->data.ownerId));

            uint32_t membersCount = faction->data.members.size() - 1;
            PackBytes(factionData, &membersCount, sizeof(membersCount));

            for (const auto& memberId : faction->data.members | std::views::keys)
            {
                if (memberId == faction->data.ownerId)
                    continue;
                PackBytes(factionData, &memberId, sizeof(memberId));
            }

            uint32_t entitiesCount = faction->colonists.size();
            PackBytes(factionData, &entitiesCount, sizeof(entitiesCount));

            for (const auto& kvp : faction->colonists)
            {
                uint16_t colonistId = kvp.first;
                Colonist* colonist = kvp.second.get();

                PackBytes(factionData, &colonistId, sizeof(colonistId));

                uint32_t entityNameLength = colonist->entityData.name.size();
                PackBytes(factionData, &entityNameLength, sizeof(entityNameLength));
                PackBytes(factionData, colonist->entityData.name.data(), entityNameLength);

                PackBytes(factionData, &colonist->entityData.position.x, sizeof(colonist->entityData.position.x));
                PackBytes(factionData, &colonist->entityData.position.y, sizeof(colonist->entityData.position.y));
            }
        }

        server->netInterface->SendToClient(to, factionData, ENET_PACKET_FLAG_RELIABLE);
    }

    void World::BroadcastFactionData(uint16_t factionId)
    {
        std::vector<uint8_t> factionData{};
        factionData.push_back(FACTION_DATA_PACKET);

        if (!factions.contains(factionId))
            return;

        uint32_t factionsCount = 1;
        PackBytes(factionData, &factionsCount, sizeof(factionsCount));

        Faction* faction = factions[factionId].get();

        PackBytes(factionData, &faction->data.id, sizeof(faction->data.id));

        uint32_t factionNameLength = faction->data.name.size();
        PackBytes(factionData, &factionNameLength, sizeof(factionNameLength));
        PackBytes(factionData, faction->data.name.data(), factionNameLength);

        PackBytes(factionData, &faction->data.ownerId, sizeof(faction->data.ownerId));

        uint32_t membersCount = faction->data.members.size() - 1;
        PackBytes(factionData, &membersCount, sizeof(membersCount));

        for (const auto& memberId : faction->data.members | std::views::keys)
        {
            if (memberId == faction->data.ownerId)
                continue;
            PackBytes(factionData, &memberId, sizeof(memberId));
        }

        uint32_t entitiesCount = faction->colonists.size();
        PackBytes(factionData, &entitiesCount, sizeof(entitiesCount));

        for (const auto& kvp : faction->colonists)
        {
            uint16_t colonistId = kvp.first;
            Colonist* colonist = kvp.second.get();

            PackBytes(factionData, &colonistId, sizeof(colonistId));

            uint32_t entityNameLength = colonist->entityData.name.size();
            PackBytes(factionData, &entityNameLength, sizeof(entityNameLength));
            PackBytes(factionData, colonist->entityData.name.data(), entityNameLength);

            PackBytes(factionData, &colonist->entityData.position.x, sizeof(colonist->entityData.position.x));
            PackBytes(factionData, &colonist->entityData.position.y, sizeof(colonist->entityData.position.y));
        }

        for (const auto& peer : server->GetPeers() | std::views::values)
            server->netInterface->SendToClient(peer, factionData, ENET_PACKET_FLAG_RELIABLE);
    }
}

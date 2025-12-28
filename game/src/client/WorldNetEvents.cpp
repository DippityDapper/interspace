#include "game/client/Colonist.hpp"
#include "game/client/World.hpp"

#include "game/network/NetworkPackets.hpp"

namespace Game::Client
{
    void World::RegisterNetEvents()
    {
        client->ConnectToEvent(WORLD_DATA_PACKET, this, &World::OnWorldDataReceived);
        client->ConnectToEvent(FACTION_DATA_PACKET, this, &World::OnFactionDataReceived);
    }

    void World::OnWorldDataReceived(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;

        worldData->worldSizeX = UnpackUint16(data, offset);
        worldData->worldSizeY = UnpackUint16(data, offset);
        uint32_t worldNameLength = UnpackUint32(data, offset);
        worldName = UnpackString(data, offset, worldNameLength);
    }

    void World::OnFactionDataReceived(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;

        uint32_t factionCount = UnpackUint32(data, offset);

        for (uint32_t i = 0; i < factionCount; i++)
        {
            uint16_t factionId = UnpackUint16(data, offset);
            factions.emplace(factionId, std::make_unique<Faction>());
            Faction* faction = factions[factionId].get();

            uint32_t factionNameLength = UnpackUint32(data, offset);
            faction->data.name = UnpackString(data, offset, factionNameLength);

            faction->data.ownerId = UnpackUint32(data, offset);
            std::string ownerUsername = client->GetUsername(faction->data.ownerId);

            faction->data.members.emplace(faction->data.ownerId, ownerUsername);

            uint32_t membersCount = UnpackUint32(data, offset);
            for (uint32_t j = 0; j < membersCount; j++)
            {
                uint32_t memberId = UnpackUint32(data, offset);
                std::string memberUsername = client->GetUsername(memberId);
                faction->data.members.emplace(memberId, memberUsername);
            }

            uint32_t entitiesCount = UnpackUint32(data, offset);
            for (uint32_t j = 0; j < entitiesCount; j++)
            {
                uint16_t colonistId = UnpackUint16(data, offset);
                faction->colonists.emplace(colonistId, std::make_unique<Colonist>());
                Colonist* colonist = faction->colonists[colonistId].get();
                colonist->entityData.id = colonistId;

                uint32_t entityNameLength = UnpackUint32(data, offset);
                colonist->entityData.name = UnpackString(data, offset, entityNameLength);

                colonist->entityData.position.x = UnpackFloat(data, offset);
                colonist->entityData.position.y = UnpackFloat(data, offset);
            }
        }
    }
}

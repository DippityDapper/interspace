#include "interspace/client/World.hpp"
#include "../../../../../igneous/include/igneous/networking/Serializer.hpp"

namespace Interspace::Client
{
    void World::OnColonistPositionDataReceived(const std::vector<uint8_t>& data)
    {
        Engine::Deserializer deserializer(data);

        uint16_t factionId = 0;
        uint16_t colonistId = 0;
        float colonistPositionX{};
        float colonistPositionY{};

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

    void World::OnColonistSelectedDataReceived(const std::vector<uint8_t>& data)
    {
        uint16_t factionId = 0;
        uint16_t colonistId = 0;
        uint32_t clientId = 0;

        Engine::Deserializer deserializer(data);
        deserializer
            >> factionId
            >> colonistId
            >> clientId;

        if (!factions.contains(factionId))
            return;

        Faction* faction = factions[factionId].get();
        if (!faction->colonists.contains(colonistId))
            return;

        Colonist* colonist = faction->colonists[colonistId].get();
        colonist->colonistData.selectedBy = clientId;

        if (clientId == client->clientId)
            colonist->sprite->SetTexture("assets/colonists/colonist_green_spritesheet.png");
        else if (clientId == 0)
            colonist->sprite->SetTexture("assets/colonists/colonist_blue_spritesheet.png");
        else
            colonist->sprite->SetTexture("assets/colonists/colonist_pink_spritesheet.png");
    }

    void World::OnColonistDeselectedAllDataReceived(const std::vector<uint8_t>& data)
    {
        Engine::Deserializer deserializer(data);
        uint16_t selectedCount = 0;
        deserializer >> selectedCount;
        for (int i = 0; i < selectedCount; i++)
        {
            uint16_t factionId = 0;
            uint16_t colonistId = 0;
            deserializer
                >> factionId
                >> colonistId;

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

    void World::OnColonistDeselectedDataReceived(const std::vector<uint8_t>& data)
    {
        uint16_t factionId = 0;
        uint16_t colonistId = 0;
        uint32_t clientId = 0;

        Engine::Deserializer deserializer(data);
        deserializer
            >> factionId
            >> colonistId
            >> clientId;

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

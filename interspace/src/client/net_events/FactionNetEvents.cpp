#include "igneous/Scene.hpp"
#include "igneous/Scenes.hpp"
#include "interspace/client/World.hpp"
#include "interspace/menus/CreateFactionMenu.hpp"
#include "interspace/network/Serializer.hpp"

namespace Interspace::Client
{
    void World::OnFactionDataReceived(const std::vector<uint8_t>& data)
    {
        Deserializer deserializer(data);

        uint32_t factionCount = 0;
        deserializer >> factionCount;

        for (uint32_t i = 0; i < factionCount; i++)
        {
            uint16_t factionId = 0;
            deserializer >> factionId;

            if (!factions.contains(factionId))
                factions.emplace(factionId, std::make_unique<Faction>());

            Faction* faction = factions[factionId].get();
            faction->data.id = factionId;

            deserializer
                >> faction->data.name
                >> faction->data.ownerId;

            uint32_t membersCount = 0;
            deserializer >> membersCount;

            for (uint32_t j = 0; j < membersCount; j++)
            {
                uint32_t memberId;
                deserializer >> memberId;
                if (!faction->data.members.contains(memberId))
                {
                    std::string memberUsername = client->GetUsername(memberId);
                    faction->data.members.emplace(memberId, memberUsername);
                }
            }

            uint32_t colonistCount = 0;
            deserializer >> colonistCount;

            for (uint32_t j = 0; j < colonistCount; j++)
            {
                uint16_t colonistId = 0;
                deserializer >> colonistId;

                if (!faction->colonists.contains(colonistId))
                    faction->colonists.emplace(colonistId, std::make_unique<Colonist>());

                Colonist* colonist = faction->colonists[colonistId].get();
                colonist->entityData.id = colonistId;

                deserializer
                    >> colonist->entityData.name
                    >> colonist->entityData.position.x
                    >> colonist->entityData.position.y
                    >> colonist->colonistData.selectedBy;

                if (colonist->colonistData.selectedBy == client->clientId)
                    colonist->sprite->SetTexture("assets/colonists/colonist_green_spritesheet.png");
                else if (colonist->colonistData.selectedBy == 0)
                    colonist->sprite->SetTexture("assets/colonists/colonist_blue_spritesheet.png");
                else
                    colonist->sprite->SetTexture("assets/colonists/colonist_pink_spritesheet.png");

                if (faction->data.members.contains(client->clientId) && faction->colonists.size() == 1)
                {
                    camera->targetPosition.x = colonist->entityData.position.x;
                    camera->targetPosition.y = colonist->entityData.position.y;
                    camera->position.x = colonist->entityData.position.x;
                    camera->position.y = colonist->entityData.position.y;
                }
            }

            if (faction->data.name != "factionless")
                continue;

            if (!faction->data.members.contains(client->clientId))
                continue;

            Engine::Scene* factionCreationMenu = Engine::Scenes::GetScene("faction_creation_menu");
            if (!factionCreationMenu)
                factionCreationMenu = Engine::Scenes::CreateScene(new CreateFactionMenu(), "faction_creation_menu");

            if (factionCreationMenu->active)
                continue;

            Engine::Scenes::LoadScene("faction_creation_menu");
        }
    }
}

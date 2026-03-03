#include "interspace/server/Faction.hpp"

namespace Interspace::Server
{
    void Faction::AddColonist(const std::string& colonistName, uint32_t colonistId, Engine::Vec2<float> colonistPosition)
    {
        if (colonists.contains(colonistId))
            return;

        colonists.emplace(colonistId, std::make_unique<Colonist>());
        Colonist* colonist = colonists[colonistId].get();

        colonist->entityData.name = colonistName;
        colonist->entityData.id = colonistId;
        colonist->entityData.position = colonistPosition;
    }

    Colonist* Faction::GetColonist(uint32_t colonistId)
    {
        if (!colonists.contains(colonistId))
            return nullptr;
        return colonists[colonistId].get();
    }
}

#include "interspace/client/Faction.hpp"

namespace Interspace::Client
{
    void Faction::Update(float delta)
    {
        for (const auto& colonist: colonists)
        {
            colonist.second->Update(delta);
        }
    }

    Colonist* Faction::GetColonist(uint32_t colonistId)
    {
        if (!colonists.contains(colonistId))
            return nullptr;
        return colonists[colonistId].get();
    }
}

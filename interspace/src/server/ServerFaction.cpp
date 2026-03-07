#include "interspace/server/ServerFaction.hpp"

#include <memory>

namespace Interspace::Server
{
    void ServerFaction::AddColonist(const std::string& colonistName, entity_id_t colonistId, Engine::Vec2<float> colonistPosition)
    {
        if (colonists.contains(colonistId))
            return;

        colonists.emplace(colonistId, std::make_unique<ServerColonist>());
        ServerColonist* colonist = colonists[colonistId].get();

        colonist->name = colonistName;
        colonist->id = colonistId;
        colonist->position = colonistPosition;
    }

    ServerColonist* ServerFaction::GetColonist(entity_id_t colonistId)
    {
        if (!colonists.contains(colonistId))
            return nullptr;
        return colonists[colonistId].get();
    }
}

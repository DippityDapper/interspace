#pragma once

#include "interspace/server/ServerColonist.hpp"
#include "interspace/world/Faction.hpp"

namespace Interspace::Server
{
    class ServerFaction : public Faction<ServerColonist>
    {
      public:
        void AddColonist(const std::string& colonistName, entity_id_t colonistId, Engine::Vec2<float> colonistPosition);
        ServerColonist* GetColonist(entity_id_t colonistId);
    };
}

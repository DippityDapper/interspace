#pragma once

#include "interspace/server/Colonist.hpp"
#include "interspace/world/FactionData.hpp"

namespace Interspace::Server
{
    class Faction
    {
      public:
        FactionData data{};
        std::map<uint32_t, std::unique_ptr<Colonist>> colonists{};

      public:
        void AddColonist(const std::string& colonistName, uint32_t colonistId, Engine::Vec2<float> colonistPosition);
        Colonist* GetColonist(uint32_t colonistId);
    };
}

#pragma once

#include "interspace/server/Colonist.hpp"
#include "interspace/world/FactionData.hpp"

namespace Interspace::Server
{
    class Faction
    {
    public:
        FactionData data{};
        std::map<uint16_t, std::unique_ptr<Colonist>> colonists{};

    public:
        void AddColonist(const std::string& colonistName, uint16_t colonistId, Engine::Vec2<float> colonistPosition);
        Colonist* GetColonist(uint16_t colonistId);
    };
}

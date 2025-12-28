#pragma once

#include "game/server/Colonist.hpp"
#include "game/server/Entity.hpp"
#include "game/world/FactionData.hpp"

namespace Game::Server
{
    class Faction
    {
    public:
        FactionData data{};
        std::map<uint16_t, std::unique_ptr<Colonist>> colonists{};
    };
}

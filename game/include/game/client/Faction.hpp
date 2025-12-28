#pragma once

#include "game/client/Colonist.hpp"
#include "game/client/Entity.hpp"
#include "game/world/FactionData.hpp"

namespace Game::Client
{
    class Faction
    {
    public:
        FactionData data{};
        std::map<uint16_t, std::unique_ptr<Colonist>> colonists{};
    };
}

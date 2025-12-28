#pragma once

#include "game/server/Entity.hpp"
#include "game/world/ColonistData.hpp"

namespace Game::Server
{
    class Colonist
    {
    public:
        EntityData entityData{};
        ColonistData colonistData{};
    };
}

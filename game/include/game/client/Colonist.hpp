#pragma once

#include "game/client/Entity.hpp"
#include "game/world/ColonistData.hpp"

namespace Game::Client
{
    class Colonist
    {
    public:
        EntityData entityData{};
        ColonistData colonistData{};
    };
}

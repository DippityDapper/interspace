#pragma once
#include "igneous/engine/Vec2.hpp"
#include "interspace/shared/game/Typedefs.hpp"

namespace Interspace
{
    struct PlayerData
    {
        client_id_t clientId = 0;
        world_id_t worldId = 0;
        Engine::Vec2<float> position{};
    };
}
#pragma once

#include "SDL3/SDL.h"

#include "common/vector/Vec2.h"

namespace Engine
{
    struct Tile
    {
        Vec2<int> gridPosition{0,0};
    };

}

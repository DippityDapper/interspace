#pragma once

#include <map>
#include <memory>

#include "dapper2d/Vec2.hpp"
#include "client/Grid.hpp"

namespace Game
{
    class Area
    {
    public:
        Grid grid{};
        Engine::Vec2<int> position{0,0};

    public:
        Area(int x, int y);
        void RenderTiles();
    };
}

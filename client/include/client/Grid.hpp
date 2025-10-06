#pragma once

#include <map>

#include "client/Tile.hpp"

namespace Game
{
    class Grid
    {
    public:
        std::map<Engine::Vec2<int>, Tile*> tiles{};
        static const int GRID_SIZE;

    public:
        ~Grid();
        void Init();
    };
}

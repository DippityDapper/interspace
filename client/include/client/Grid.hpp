#pragma once

#include <map>

#include "client/Tile.hpp"

namespace Game
{
    class Grid
    {
    public:
        std::map<Engine::Vec2<int>, Tile*> tiles{};
        Engine::Vec2<int> gridSize{0, 0};
        const Engine::Vec2<int> TILE_SIZE{64, 64};

    public:
        ~Grid();
        void Init(int gridX, int gridY);
        Engine::Vec2<int> GlobalToLocal(Engine::Vec2<float> position) const;
        Engine::Vec2<float> LocalToGlobal(Engine::Vec2<int> position) const;
    };
}

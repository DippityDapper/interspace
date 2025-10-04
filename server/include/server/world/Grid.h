#pragma once

#include <map>

#include "server/world/Tile.h"

namespace Engine
{
    class Grid
    {
    public:
        std::map<Vec2<int>, Tile> tiles{};
        Vec2<int> gridSize{0, 0};
        Vec2<int> TILE_SIZE{0, 0};

    public:
        void InitializeGrid(int gridX, int gridY, int tileX, int tileY);
        Vec2<int> GlobalToLocal(Vec2<float> position) const;
        Vec2<float> LocalToGlobal(Vec2<int> position) const;
    };
}

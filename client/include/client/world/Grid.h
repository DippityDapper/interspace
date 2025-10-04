#pragma once

#include <map>

#include "client/world/Tile.h"

namespace Engine
{
    class Grid
    {
    public:
        std::map<Vec2<int>, Tile*> tiles{};
        Vec2<int> gridSize{0, 0};
        const Vec2<int> TILE_SIZE{64, 64};

    public:
        ~Grid();
        void Init(int gridX, int gridY);
        Vec2<int> GlobalToLocal(Vec2<float> position) const;
        Vec2<float> LocalToGlobal(Vec2<int> position) const;
    };
}

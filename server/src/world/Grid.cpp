#include "server/world/Grid.h"

namespace Engine
{
    void Grid::InitializeGrid(int gridX, int gridY, int tileX, int tileY)
    {
        gridSize.x = gridX;
        gridSize.y = gridY;

        TILE_SIZE.x = tileX;
        TILE_SIZE.y = tileY;

        tiles.clear();

        for (int y = 0; y < gridY; ++y)
        {
            for (int x = 0; x < gridX; ++x)
            {
                if (x > 4 && x < 15 && y > 4 && y < 15)
                    continue;
                Vec2<int> position{x, y};
                Tile tile{position};

                tiles.emplace(position, tile);
            }
        }
    }

    Vec2<int> Grid::GlobalToLocal(Vec2<float> position) const
    {
        int dx = std::floor(position.x / TILE_SIZE.x);
        int dy = std::floor(position.y / TILE_SIZE.y);
        return {dx, dy};
    }

    Vec2<float> Grid::LocalToGlobal(Vec2<int> position) const
    {
        float dx = position.x * TILE_SIZE.x;
        float dy = position.y * TILE_SIZE.y;
        return {dx, dy};
    }
}

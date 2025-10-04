#include <random>
#include "client/world/Grid.h"

namespace Engine
{

    Grid::~Grid()
    {
        for (auto &kvp : tiles)
        {
            delete kvp.second;
        }
    }

    void Grid::Init(int gridX, int gridY)
    {
        gridSize.x = gridX;
        gridSize.y = gridY;

        tiles.clear();

        std::string texturePath = "tilesets/grass_tileset.png";
        for (int y = 0; y < gridY; ++y)
        {
            for (int x = 0; x < gridX; ++x)
            {
                Vec2<int> position{x, y};

                std::random_device rd;
                std::mt19937 gen(rd());

                std::uniform_int_distribution<> distribX(0, 7);
                std::uniform_int_distribution<> distribY(0, 3);

                int atlasX = distribX(gen);
                int atlasY = distribY(gen);

                Tile* tile = new Tile(position, texturePath, 32, 32, atlasX, atlasY); // 4 , 8
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

#pragma once

#include <string>
#include <memory>

#include "dapper2d/Vec2.hpp"

#include "client/Tiles.hpp"

namespace Engine
{
    class Sprite;
}

namespace Game
{
    struct TileSaveData
    {
        int tileX;
        int tileY;
        int tileType;
    };

    class Tile
    {
    public:
        static int TILE_SIZE;

        std::unique_ptr<Engine::Sprite> sprite;
        Tiles::Type type = Tiles::GRASS_1;

    public:
        explicit Tile(const std::string& filePath);
        Tile(const std::string& filePath, float w, float h, int x, int y);
        Tile(const Tile& tile);
        Tile& operator=(const Tile&) = delete;

        void Render(int tilePositionX, int tilePositionY) const;
    };
}

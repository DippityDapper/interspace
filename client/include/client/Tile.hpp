#pragma once

#include <string>

#include "dapper2d/Vec2.hpp"

#include "client/Tiles.hpp"

namespace Engine
{
    class Sprite;
}

namespace Game
{
    class Tile
    {
    public:
        static int TILE_SIZE;

        Engine::Sprite* sprite = nullptr;
        Tiles::Type type = Tiles::GRASS_1;

    public:
        explicit Tile(const std::string& texturePath);
        Tile(const std::string& texturePath, float w, float h, int x, int y);
        ~Tile();
        Tile(const Tile& tile);
        Tile& operator=(const Tile&) = delete;

        void Render(int tilePositionX, int tilePositionY) const;
    };
}

#pragma once

#include <string>

#include "dapper2d/Vec2.hpp"

namespace Engine
{
    class Sprite;
}

namespace Game
{
    class Tile
    {
    public:
        Engine::Vec2<int> gridPosition{0,0};
        Engine::Sprite* sprite = nullptr;

    public:
        Tile(Engine::Vec2<int> _gridPosition, std::string& texturePath);
        Tile(Engine::Vec2<int> _gridPosition, std::string& texturePath, float w, float h, int x, int y);
        ~Tile();
        Tile(const Tile&) = delete;
        Tile& operator=(const Tile&) = delete;

        void Render(int offsetX, int offsetY) const;
    };
}

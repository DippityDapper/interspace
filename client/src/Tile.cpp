#include "client/Tile.hpp"

#include "SDL3/SDL.h"

#include "dapper2d/Renderer.hpp"
#include "dapper2d/Sprite.hpp"

namespace Game
{
    Tile::Tile(Engine::Vec2<int> _gridPosition, std::string& texturePath)
    {
        gridPosition.x = _gridPosition.x;
        gridPosition.y = _gridPosition.y;
        sprite = new Engine::Sprite(texturePath);
    }

    Tile::Tile(Engine::Vec2<int> _gridPosition, std::string &texturePath, float w, float h, int x, int y)
    {
        gridPosition.x = _gridPosition.x;
        gridPosition.y = _gridPosition.y;
        sprite = new Engine::Sprite(texturePath, w, h, x, y);
    }

    Tile::~Tile()
    {
        delete sprite;
    }

    void Tile::Render(int offsetX, int offsetY) const
    {
        if (!sprite)
            return;

        if (sprite->w <= 0 || sprite->h <= 0)
            return;

        Engine::Vec2<float> position{(float)(gridPosition.x + offsetX) * sprite->w, (float)(gridPosition.y + offsetY) * sprite->h};
        Engine::Renderer::BufferAdd(position, sprite);
    }
}
#include "client/Tile.hpp"

#include "SDL3/SDL.h"

#include "dapper2d/Renderer.hpp"
#include "dapper2d/Sprite.hpp"

namespace Game
{
    int Tile::TILE_SIZE = 0;

    Tile::Tile(const Tile &tile)
    {
        sprite = tile.sprite;
    }

    Tile::Tile(const std::string& texturePath)
    {
        sprite = new Engine::Sprite(texturePath);
    }

    Tile::Tile(const std::string &texturePath, float w, float h, int x, int y)
    {
        sprite = new Engine::Sprite(texturePath, w, h, x, y);
    }

    Tile::~Tile()
    {
        delete sprite;
    }

    void Tile::Render(int tilePositionX, int tilePositionY) const
    {
        if (!sprite)
            return;

        if (sprite->tileW <= 0 || sprite->tileH <= 0)
            return;
        Engine::Vec2<float> position{(float)tilePositionX * Tile::TILE_SIZE, (float)tilePositionY * Tile::TILE_SIZE};
        Engine::Renderer::BufferAdd(position, sprite);
    }
}
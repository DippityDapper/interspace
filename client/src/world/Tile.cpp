#include "client/world/Tile.h"

namespace Engine
{
    Tile::Tile(Vec2<int> _gridPosition, std::string& texturePath)
    {
        gridPosition.x = _gridPosition.x;
        gridPosition.y = _gridPosition.y;
        sprite = new Sprite(texturePath);
    }

    Tile::Tile(Vec2<int> _gridPosition, std::string &texturePath, float w, float h, int x, int y)
    {
        gridPosition.x = _gridPosition.x;
        gridPosition.y = _gridPosition.y;
        sprite = new Sprite(texturePath, w, h, x, y);
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

        Vec2<float> position{(float)(gridPosition.x + offsetX) * sprite->w, (float)(gridPosition.y + offsetY) * sprite->h};
        Renderer::BufferAdd(position, sprite);
    }
}
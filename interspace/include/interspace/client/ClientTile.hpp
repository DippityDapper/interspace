#pragma once

#include "igneous/resources/Sprite.hpp"
#include "interspace/world/Tile.hpp"

namespace Interspace::Client
{
    class ClientTile : public Tile
    {
      public:
        std::unique_ptr<Engine::Sprite> sprite = nullptr;
    };
}

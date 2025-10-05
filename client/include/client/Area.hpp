#pragma once

#include <map>
#include <memory>

#include "enet/enet.h"

#include "client/Grid.hpp"
#include "client/ClientEntity.hpp"

namespace Game
{
    class Area
    {
    public:
        Engine::Grid grid{};
        Engine::Vec2<int> position{0,0};

    public:
        Area(int x, int y);
        void RenderTiles();
    };
}

#pragma once

#include <map>
#include <memory>

#include "enet/enet.h"

#include "client/world/Grid.h"

#include "client/entity/ClientEntity.h"
#include "client/networking/NetworkManager.h"

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

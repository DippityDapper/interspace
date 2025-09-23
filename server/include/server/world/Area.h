#pragma once

#include <map>
#include <memory>

#include "enet/enet.h"

#include "server/world/Grid.h"

namespace Game
{
    class Area
    {
    public:
        Engine::Grid grid{};

    public:
        void Init();
        void Update(float delta);
    };
}

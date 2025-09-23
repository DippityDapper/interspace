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

    public:
        void Init(SDL_Renderer* renderer);

    public:
        void RenderTiles(SDL_Renderer *renderer, Engine::Camera& camera);
    };
}

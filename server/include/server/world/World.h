#pragma once

#include <queue>

#include "SDL3/SDL.h"

#include "server/astar/AStar.h"

#include "server/world/Area.h"
#include "server/entity/ServerEntity.h"
#include "server/networking/NetworkManager.h"

namespace Game
{
    class World
    {
    public:
        Engine::Vec2<int> currentAreaPosition{0, 0};
        std::map<Engine::Vec2<int>, Area> areas{};
        Engine::Vec2<int> worldSize{0, 0};
        Engine::AStar astar{};
        std::map<int, std::unique_ptr<ServerEntity>> entities{};

        std::queue<int> entityPositionUpdated{};

    public:
        void Init(Engine::Vec2<int> _worldSize);
        void Update(float delta);
        void NetworkUpdate(Engine::NetworkManager* nm);

        Area* GetCurrentArea();
        void CleanEntities();

        void OnClientConnected(int clientId, Engine::NetworkManager*);
        void OnClientDisconnected(int clientId);

    private:
        void HandlePositionPacket(ENetEvent &enetEvent);
    };
}

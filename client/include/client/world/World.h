#pragma once

#include <map>

#include "SDL3/SDL.h"

#include "common/packets/Packets.h"

#include "client/world/Area.h"
#include "client/camera/Camera.h"

namespace Game
{
    class World
    {
    public:
        SDL_Renderer* renderer = nullptr;

        Engine::Vec2<int> currentAreaPosition{0, 0};
        std::map<Engine::Vec2<int>, Area> areas{};
        Engine::Vec2<int> worldSize{0, 0};

        std::map<int, std::unique_ptr<ClientEntity>> entities{};

    public:
        SDL_AppResult Init(Engine::Vec2<int> _worldSize, SDL_Renderer* renderer);
        Area* GetCurrentArea();
        void Render(SDL_Renderer* renderer, Engine::Camera& camera);
        void RenderEntities(SDL_Renderer *renderer, Engine::Camera& camera);
        void CleanEntities();

    private:
        void OnClientDisconnect(int clientId);
        void HandlePositionPacket(ENetEvent& event);
        void HandleCreateClientEntityPacket(ENetEvent& event);
    };
}

#pragma once

#include <map>

#include "common/packets/Packets.h"
#include "common/parser/CFGParser.h"

#include "client/entity/ClientEntity.h"
#include "client/networking/NetworkManager.h"
#include "client/world/World.h"

namespace Game
{
    class Game
    {
    private:
        bool running = true;
        Engine::NetworkManager networkManager{};

        World world{};

    public:
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;

        uint64_t lastTick = 0;
        uint64_t currentTick = 0;
        float deltaTime = 0;

        Engine::Camera camera{};

    private:
        SDL_AppResult InitSDL();
        SDL_AppResult InitNetworking();

    public:
        void Init();
        void Update();
        void Render();
        void Clean();
        void HandleEvents();
    };
}

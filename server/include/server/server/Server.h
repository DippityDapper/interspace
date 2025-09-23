#pragma once

#include <thread>
#include <map>
#include <string>
#include <iostream>

#include "common/packets/Packets.h"
#include "common/parser/CFGParser.h"

#include "server/entity/ServerEntity.h"
#include "server/networking/NetworkManager.h"
#include "server/world/World.h"
#include "server/data/GameData.h"
#include "server/world/Grid.h"

namespace Game
{
    class Server
    {
    private:
        bool running = true;
        std::thread terminalThread;

        Engine::NetworkManager networkManager{};
        GameData data{};

        World world{};

    public:
        uint64_t lastTick = 0;
        uint64_t currentTick = 0;
        float deltaTime = 0;
        float networkTimer = 0;

    private:
        SDL_AppResult InitSDL();
        SDL_AppResult InitNetworking();

    public:
        void Init();
        void Update();
        void Clean();
        void HandleEvents();

        void TerminalThread();
    };
}

#include "server/server/Server.h"

namespace Game
{
    void Server::Init()
    {
        if (InitSDL() == SDL_APP_FAILURE)
        {
            running = false;
            return;
        }

        data.Init();

        if (InitNetworking() == SDL_APP_FAILURE)
        {
            running = false;
            return;
        }

        terminalThread = std::thread(&Server::TerminalThread, this);

        world.Init({25, 25});
        // SDL_Log("Username : %s", GameData::GetName(1).c_str());
    }

    SDL_AppResult Server::InitSDL()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("SDL init failed: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        return SDL_APP_CONTINUE;
    }

    SDL_AppResult Server::InitNetworking()
    {
        if (enet_initialize() != 0)
        {
            SDL_Log("An error occurred while initializing ENet: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        if (networkManager.Init() == SDL_APP_FAILURE)
        {
            return SDL_APP_FAILURE;
        }

        Engine::CFGParser::LoadConfig("server-properties.cfg");
        int port = Engine::CFGParser::GetInt("server-properties.cfg", "server-port");
        bool isLocalOnly = Engine::CFGParser::GetBool("server-properties.cfg", "local-host-only");

        if (networkManager.CreateServer(port, isLocalOnly) == SDL_APP_FAILURE)
        {
            return SDL_APP_FAILURE;
        }

        return SDL_APP_CONTINUE;
    }

    void Server::HandleEvents()
    {
        networkManager.HandleNetworkEvents();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
    }

    void Server::Update()
    {
        while (running)
        {
            HandleEvents();

            lastTick = currentTick;
            currentTick = SDL_GetTicks();
            deltaTime = (float)(currentTick - lastTick) / 1000.0f;

            world.Update(deltaTime);

            networkTimer += deltaTime;
            if (networkTimer > 0.1f)
            {
                world.NetworkUpdate(&networkManager);
                networkTimer -= 0.1f;
            }
        }
    }

    void Server::Clean()
    {
        if (terminalThread.joinable())
            terminalThread.join();

        world.CleanEntities();

        enet_host_destroy(networkManager.server);
        enet_deinitialize();

        SDL_Quit();
    }

    void Server::TerminalThread()
    {
        std::string line;
        while (running)
        {
            if (std::getline(std::cin, line))
            {
                if (line.empty() || line == "q")
                {
                    running = false;
                }
            }
        }
    }
}

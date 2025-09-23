#include "client/game/Game.h"

namespace Game
{
    void Game::Init()
    {
        if (InitSDL() == SDL_APP_FAILURE)
        {
            running = false;
            return;
        }

        if (InitNetworking() == SDL_APP_FAILURE)
        {
            running = false;
            return;
        }

        world.Init({1, 1}, renderer);
    }

    SDL_AppResult Game::InitSDL()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("SDL init failed: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        if (!SDL_CreateWindowAndRenderer("client", 640, 360, SDL_WINDOW_RESIZABLE, &window, &renderer))
        {
            SDL_Log("Window/Renderer creation failed: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        return SDL_APP_CONTINUE;
    }

    SDL_AppResult Game::InitNetworking()
    {
        if (networkManager.Init() == SDL_APP_FAILURE)
        {
            return SDL_APP_FAILURE;
        }
        if (networkManager.CreateClient() == SDL_APP_FAILURE)
        {
            return SDL_APP_FAILURE;
        }

        Engine::CFGParser::LoadConfig("server-properties.cfg");

        std::string ip = Engine::CFGParser::GetString("server-properties.cfg", "server-ip");
        int port = Engine::CFGParser::GetInt("server-properties.cfg", "server-port");

        if (networkManager.ConnectToServer(port, ip) == SDL_APP_FAILURE)
        {
            return SDL_APP_FAILURE;
        }

        return SDL_APP_CONTINUE;
    }

    void Game::HandleEvents()
    {
        if (!networkManager.HandleNetworkEvents())
            running = false;

        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent))
        {
            if (sdlEvent.type == SDL_EVENT_QUIT)
            {
                running = false;
                return;
            }

            if (sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (sdlEvent.button.button == SDL_BUTTON_LEFT)
                {
                    float mouseX = sdlEvent.button.x;
                    float mouseY = sdlEvent.button.y;

                    Engine::PositionPacket positionPacket;
                    positionPacket.type = Engine::PACKET_POSITION;
                    positionPacket.clientId = networkManager.clientId;

                    int winW = 0, winH = 0;
                    SDL_GetWindowSize(window, &winW, &winH);

                    positionPacket.x = camera.position.x + mouseX / camera.zoom;
                    positionPacket.y = camera.position.y + mouseY / camera.zoom;

                    ENetPacket* packet = enet_packet_create(&positionPacket, sizeof(Engine::PositionPacket), ENET_PACKET_FLAG_RELIABLE);
                    enet_peer_send(networkManager.server, 0, packet);
                }
            }
            if (sdlEvent.type == SDL_EVENT_KEY_DOWN)
            {
                if (sdlEvent.key.key == SDLK_ESCAPE)
                {
                    running = false;
                    networkManager.Disconnect();
                    return;
                }
            }
            camera.HandleEvents(sdlEvent);
        }
    }

    void Game::Update()
    {
        while (running)
        {
            HandleEvents();

            if (networkManager.clientId < 0)
                continue;

            lastTick = currentTick;
            currentTick = SDL_GetTicks();
            deltaTime = (float)(currentTick - lastTick) / 1000.0f;

            camera.Update(deltaTime);

            Render();
        }
    }

    void Game::Render()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        world.Render(renderer, camera);

        SDL_RenderPresent(renderer);
    }

    void Game::Clean()
    {
        world.CleanEntities();
        Engine::ResourceLoader::UnloadAll();

        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        networkManager.Clean();

        SDL_Quit();
    }
}

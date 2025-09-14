#include "Game.h"
#include "../../common/CFGParser.h"

/*
 * TODO : Refactor everything
 * 1. Move all networking to the network manager
 *      1. Make the packet sending and catching more flexible X
 *      2. Move port and address to a readable config
 * 2. Move some game logic to another class, perhaps something like world or map X
*/

AppState* Game::state = nullptr;

void Game::Init()
{
    state = new AppState();

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

    world.Init({1, 1});
}

SDL_AppResult Game::InitSDL()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("client", 640, 360, SDL_WINDOW_RESIZABLE, &state->window, &state->renderer))
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

    CFGParser::LoadConfig("server-properties.cfg");

    const char* ip = CFGParser::GetString("server-properties.cfg", "server-ip");
    int port = CFGParser::GetInt("server-properties.cfg", "server-port");

    if (networkManager.ConnectToServer(port, ip) == SDL_APP_FAILURE)
    {
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

bool Game::IsRunning() const
{
    return running;
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

                PositionPacket positionPacket;
                positionPacket.type = PACKET_POSITION;
                positionPacket.clientId = networkManager.clientId;
                positionPacket.x = mouseX;
                positionPacket.y = mouseY;

                ENetPacket* packet = enet_packet_create(&positionPacket, sizeof(PositionPacket), ENET_PACKET_FLAG_RELIABLE);
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
    }
}

void Game::Update()
{
    if (networkManager.clientId < 0)
        return;

    state->lastTick = state->currentTick;
    state->currentTick = SDL_GetTicks();
    state->deltaTime = (float)(state->currentTick - state->lastTick) / 1000.0f;
}

void Game::Render()
{
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state->renderer);

    world.Render(state->renderer);

    SDL_RenderPresent(state->renderer);
}

void Game::Clean()
{
    SDL_DestroyWindow(state->window);
    SDL_DestroyRenderer(state->renderer);
    delete(state);

    world.CleanEntities();
    ResourceLoader::UnloadAll();

    enet_peer_reset(networkManager.server);
    enet_host_destroy(networkManager.client);
    enet_deinitialize();

    SDL_Quit();
}

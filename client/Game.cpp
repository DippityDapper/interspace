#include "Game.h"

void Game::Init()
{
    state = new AppState();

    if (InitSDL() == SDL_APP_FAILURE)
    {
        running = false;
        return;
    }

    if (InitEnet() == SDL_APP_FAILURE)
    {
        running = false;
        return;
    }

    if (CreateClient() == SDL_APP_FAILURE)
    {
        running = false;
        return;
    }

    if (ConnectToServer(33333, "localhost") == SDL_APP_FAILURE)
    {
        running = false;
        return;
    }
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

SDL_AppResult Game::InitEnet()
{
    if (enet_initialize() != 0)
    {
        SDL_Log("An error occurred while initializing ENet: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult Game::CreateClient()
{
    client = enet_host_create(nullptr, 1, 2, 0, 0);

    if (!client)
    {
        SDL_Log("An error occurred while trying to create an ENet client ip: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult Game::ConnectToServer(int port, const char* ip)
{
    ENetAddress address;

    enet_address_set_host(&address, ip);
    address.port = port;

    host = enet_host_connect(client, &address, 2, 0);
    if (!host)
    {
        SDL_Log("No available peers for initiating connection: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_Log("Attempting to connect to server...");

    ENetEvent event;
    bool connected = false;
    uint32_t connectionTimeout = SDL_GetTicks() + 5000;

    while (!connected && SDL_GetTicks() < connectionTimeout)
    {
        while (enet_host_service(client, &event, 100) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                    SDL_Log("Connection to server succeeded");
                    connected = true;
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    SDL_Log("Connection to server failed");
                    return SDL_APP_FAILURE;
                default:
                    break;
            }
        }
    }

    if (!connected)
    {
        SDL_Log("Connection to server timed out");
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

void Game::SendInputPacket(bool up, bool down, bool left, bool right)
{
    InputPacket inputPacket;
    inputPacket.type = PACKET_INPUT;
    inputPacket.up = up;
    inputPacket.down = down;
    inputPacket.left = left;
    inputPacket.right = right;

    ENetPacket* packet = enet_packet_create(&inputPacket, sizeof(InputPacket), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(host, 0, packet);
}

bool Game::IsRunning() const
{
    return running;
}

void Game::Run()
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        if (sdlEvent.type == SDL_EVENT_QUIT)
        {
            running = false;
            return;
        }
        if (sdlEvent.type == SDL_EVENT_KEY_DOWN)
        {
            if (sdlEvent.key.key == SDLK_ESCAPE)
            {
                running = false;
                return;
            }

            if (sdlEvent.key.key == SDLK_W)
                inputState[0] = true;
            if (sdlEvent.key.key == SDLK_S)
                inputState[1] = true;
            if (sdlEvent.key.key == SDLK_A)
                inputState[2] = true;
            if (sdlEvent.key.key == SDLK_D)
                inputState[3] = true;
        }
        if (sdlEvent.type == SDL_EVENT_KEY_UP)
        {
            if (sdlEvent.key.key == SDLK_W)
                inputState[0] = false;
            if (sdlEvent.key.key == SDLK_S)
                inputState[1] = false;
            if (sdlEvent.key.key == SDLK_A)
                inputState[2] = false;
            if (sdlEvent.key.key == SDLK_D)
                inputState[3] = false;
        }
    }

    bool inputChanged = false;
    for (int i = 0; i < 4; i++)
    {
        if (inputState[i] != lastInputState[i])
        {
            inputChanged = true;
            lastInputState[i] = inputState[i];
        }
    }

    if (inputChanged)
    {
        SendInputPacket(inputState[0], inputState[1], inputState[2], inputState[3]);
    }

    ENetEvent netEvent;
    while (enet_host_service(client, &netEvent, 0) > 0)
    {
        switch (netEvent.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
            {
                PacketType* packetType = (PacketType*)netEvent.packet->data;

                if (*packetType == PACKET_STATE)
                {
                    StatePacket* statePacket = (StatePacket*)netEvent.packet->data;

                    if (entities.find(statePacket->entityId) == entities.end())
                    {
                        entities[statePacket->entityId] = new ClientEntity(state->renderer, "player/1.png", statePacket->x, statePacket->y);
                        SDL_Log("Entity added: %d", statePacket->entityId);
                    }

                    entities[statePacket->entityId]->SetPosition(statePacket->x, statePacket->y);
                }

                enet_packet_destroy(netEvent.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                SDL_Log("Disconnected from server");
                running = false;
                return;
            }
            default:
                break;
        }
    }

    state->lastTick = state->currentTick;
    state->currentTick = SDL_GetTicks();
    state->deltaTime = (float)(state->currentTick - state->lastTick) / 1000.0f;

    // Render
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state->renderer);

    for (auto& pair : entities)
    {
        pair.second->Render(state->renderer);
    }

    SDL_RenderPresent(state->renderer);
}

void Game::Quit()
{
    SDL_DestroyWindow(state->window);
    SDL_DestroyRenderer(state->renderer);
    delete(state);

    for (auto& pair : entities)
    {
        delete pair.second;
    }
    ResourceLoader::UnloadAll();

    enet_host_destroy(client);
    enet_deinitialize();

    SDL_Quit();
}

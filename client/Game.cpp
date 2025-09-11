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

    //const char* ip = "192.168.1.172";
    const char* ip = "localhost";
    if (ConnectToServer(33333, ip) == SDL_APP_FAILURE)
    {
        running = false;
        return;
    }

    SDL_Texture* tileTexture = ResourceLoader::LoadTexture(state->renderer, "tiles/station_floor_tile_1.png");
    for (int x = 0; x < 25; x++)
    for (int y = 0; y < 25; y++)
    {
        Position gridPosition{(float)x, (float)y};
        Tile tile{gridPosition, tileTexture};
        grid.emplace(gridPosition, tile);
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
    SDL_Log("Address: %u", address.host);
    address.port = port;

    server = enet_host_connect(client, &address, 2, 0);
    if (!server)
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

    ENetPacket* packet = enet_packet_create(nullptr, sizeof(PACKET_CONNECT), ENET_PACKET_FLAG_RELIABLE);
    packet->data[0] = PACKET_CONNECT;
    enet_peer_send(server, 0, packet);

    return SDL_APP_CONTINUE;
}

bool Game::IsRunning() const
{
    return running;
}

void Game::Event(SDL_Event sdlEvent)
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
            positionPacket.clientId = clientId;
            positionPacket.x = mouseX;
            positionPacket.y = mouseY;

            ENetPacket* packet = enet_packet_create(&positionPacket, sizeof(PositionPacket), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(server, 0, packet);
        }
    }
    if (sdlEvent.type == SDL_EVENT_KEY_DOWN)
    {
        if (sdlEvent.key.key == SDLK_ESCAPE)
        {
            running = false;

            DisconnectPacket disconnectPacket;
            disconnectPacket.type = PACKET_DISCONNECT;
            disconnectPacket.clientId = clientId;
            SDL_Log("Peer disconnected: %d", clientId);

            ENetPacket* packet = enet_packet_create(&disconnectPacket, sizeof(DisconnectPacket), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(server, 0, packet);
            enet_host_flush(client);

            return;
        }
    }
}

void Game::PacketReceived(ENetEvent enetEvent)
{
    PacketType* p_packetType = (PacketType*)enetEvent.packet->data;
    PacketType packetType = *p_packetType;

    if (packetType == PACKET_STATE)
    {
        StatePacket* statePacket = (StatePacket*)enetEvent.packet->data;
        entities[statePacket->clientId]->SetPosition(statePacket->x, statePacket->y);
    }
    if (packetType == PACKET_CLIENT_DATA)
    {
        ClientDataPacket* clientDataPacket = (ClientDataPacket*)enetEvent.packet->data;
        SDL_Log("Client id: %d", clientDataPacket->clientId);

        clientId = clientDataPacket->clientId;

        ClientDataPacket createClientEntityPacket;
        createClientEntityPacket.type = PACKET_CREATE_CLIENT_ENTITY;
        createClientEntityPacket.clientId = clientId;

        ENetPacket* packet = enet_packet_create(&createClientEntityPacket, sizeof(ClientDataPacket), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(server, 0, packet);
    }
    if (packetType == PACKET_DISCONNECT)
    {
        DisconnectPacket* disconnectPacket = (DisconnectPacket*)enetEvent.packet->data;
        int peerId = disconnectPacket->clientId;
        SDL_Log("Peer disconnected: %d", peerId);

        ClientEntity* entity = entities[peerId];
        entities.erase(peerId);
        delete(entity);
    }
    if (packetType == PACKET_CREATE_CLIENT_ENTITY)
    {
        StatePacket* statePacket = (StatePacket*)enetEvent.packet->data;
        SDL_Log("Client entity created for: %d", statePacket->clientId);

        ClientEntity* entity = new ClientEntity(state->renderer, "player/crew_1.png", statePacket->x, statePacket->y);
        entities[statePacket->clientId] = entity;
    }

    enet_packet_destroy(enetEvent.packet);
}

void Game::EnetEvent(ENetEvent enetEvent)
{
    switch (enetEvent.type)
    {
        case ENET_EVENT_TYPE_RECEIVE:
        {
            PacketReceived(enetEvent);
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Disconnected from server.");
            running = false;
            break;
        }
        default:
            break;
    }
}

void Game::Run()
{
    ENetEvent enetEvent;
    while (enet_host_service(client, &enetEvent, 0) > 0)
    {
        EnetEvent(enetEvent);
    }

    if (clientId < 0)
        return;

    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        Event(sdlEvent);
    }

    state->lastTick = state->currentTick;
    state->currentTick = SDL_GetTicks();
    state->deltaTime = (float)(state->currentTick - state->lastTick) / 1000.0f;

    SDL_SetRenderDrawColor(state->renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state->renderer);

    for (auto& kvp : grid)
    {
        kvp.second.Render(state->renderer);
    }

    for (auto& kvp : entities)
    {
        ClientEntity* entity = kvp.second;
        entity->Render(state->renderer);
    }

    SDL_RenderPresent(state->renderer);
}

void Game::Quit()
{
    SDL_DestroyWindow(state->window);
    SDL_DestroyRenderer(state->renderer);
    delete(state);

    for (auto& kvp : entities)
    {
        ClientEntity* entity = kvp.second;
        delete entity;
    }
    ResourceLoader::UnloadAll();

    enet_peer_reset(server);
    enet_host_destroy(client);
    enet_deinitialize();

    SDL_Quit();
}

#include "Server.h"

void Server::Init()
{
    if (InitRendererAndWindow() == SDL_APP_FAILURE)
    {
        running = false;
        return;
    }

    if (InitEnet() == SDL_APP_FAILURE)
    {
        running = false;
        return;
    }

    if (CreateServer("localhost") == SDL_APP_FAILURE)
    {
        running = false;
        return;
    }
}

SDL_AppResult Server::InitRendererAndWindow()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->window = SDL_CreateWindow("host", 640, 360, 0);
    if (!state->window)
    {
        SDL_Log("Window/Renderer creation failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult Server::InitEnet()
{
    if (enet_initialize() != 0)
    {
        SDL_Log("An error occurred while initializing ENet: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult Server::CreateServer(const char *host)
{
    ENetAddress address;
    enet_address_set_host(&address, host);
    address.port = 33333;

    server = enet_host_create(&address, 32, 2, 0, 0);
    if (!server)
    {
        SDL_Log("An error occurred while trying to create an ENet server host: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_Log("Server started on port %d", address.port);
    return SDL_APP_CONTINUE;
}

void Server::BroadcastEntityState(ServerEntity *entity, int entityId)
{
    StatePacket statePacket;
    statePacket.type = PACKET_STATE;
    statePacket.entityId = entityId;
    Position pos = entity->GetPosition();
    statePacket.x = pos.x;
    statePacket.y = pos.y;

    ENetPacket* packet = enet_packet_create(&statePacket, sizeof(StatePacket), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(server, 0, packet);
}

bool Server::IsRunning()
{
    return running;
}

void Server::Run()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            running = false;
        }
        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (event.key.key == SDLK_ESCAPE)
            {
                running = false;
            }
        }
    }

    // Handle network events
    ENetEvent netEvent;
    while (enet_host_service(server, &netEvent, 0) > 0)
    {
        switch (netEvent.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                ClientConnected(netEvent.peer);
                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
            {
                PacketType* packetType = (PacketType*)netEvent.packet->data;

                if (*packetType == PACKET_INPUT)
                {
                    InputPacket* inputPacket = (InputPacket*)netEvent.packet->data;

                    // Find the entity associated with this host
                    for (auto& clientEntity : peerEntities)
                    {
                        if (clientEntity.peer == netEvent.peer)
                        {
                            // Apply input to entity
                            clientEntity.entity->SetInputs(inputPacket->up, inputPacket->down, inputPacket->left, inputPacket->right);
                            break;
                        }
                    }
                }

                enet_packet_destroy(netEvent.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                SDL_Log("Client disconnected");

                // Remove client entity
                for (auto it = peerEntities.begin(); it != peerEntities.end(); ++it)
                {
                    if (it->peer == netEvent.peer)
                    {
                        delete it->entity;
                        peerEntities.erase(it);
                        break;
                    }
                }

                netEvent.peer->data = nullptr;
                break;
            }
        }
    }

    state->lastTick = state->currentTick;
    state->currentTick = SDL_GetTicks();
    state->deltaTime = (state->currentTick - state->lastTick) / 1000.0f;

    // Update all client entities
    for (auto& clientEntity : peerEntities)
    {
        Position oldPos = clientEntity.entity->GetPosition();
        clientEntity.entity->Update(state->deltaTime);
        Position newPos = clientEntity.entity->GetPosition();

        // Only broadcast if position changed
        if (oldPos.x != newPos.x || oldPos.y != newPos.y)
        {
            BroadcastEntityState(clientEntity.entity, clientEntity.entityId);
        }
    }
}

void Server::Quit()
{
    for (auto& clientEntity : peerEntities)
    {
        delete clientEntity.entity;
    }

    SDL_DestroyWindow(state->window);
    delete(state);

    enet_host_destroy(server);
    enet_deinitialize();

    SDL_Quit();
}

void Server::ClientConnected(ENetPeer* peer)
{
    SDL_Log("A client connected from %x:%u", peer->address.host, peer->address.port);

    // Create entity for new client
    ServerEntity* newEntity = new ServerEntity(64, 64);
    PeerEntity peerEntity{};
    peerEntity.entity = newEntity;
    peerEntity.peer = peer;
    peerEntity.entityId = nextEntityId++;

    peerEntities.push_back(peerEntity);

    // Send initial state to new client
    BroadcastEntityState(newEntity, peerEntity.entityId);
}

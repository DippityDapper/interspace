#include <string>
#include <iostream>
#include "Server.h"

void Server::Init()
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

    if (CreateServer("localhost") == SDL_APP_FAILURE)
    {
        running = false;
        return;
    }

    terminalThread = std::thread(&Server::TerminalThread, this);
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

void Server::BroadcastEntityState(ServerEntity *entity, int clientId)
{
    StatePacket statePacket;
    statePacket.type = PACKET_STATE;
    statePacket.clientId = clientId;
    Position pos = entity->GetPosition();
    statePacket.x = pos.x;
    statePacket.y = pos.y;

    ENetPacket* packet = enet_packet_create(&statePacket, sizeof(StatePacket), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(server, 0, packet);
}

bool Server::IsRunning() const
{
    return running;
}

void Server::Event(SDL_Event event)
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

void Server::DisconnectClient(int clientId)
{
    if (peers.contains(clientId))
    {
        peers.erase(clientId);
    }
    if (peerEntities.contains(clientId))
    {
        PeerEntity entity = peerEntities[clientId];
        delete entity.entity;
        peerEntities.erase(clientId);
    }

    ClientDataPacket clientDataPacket;
    clientDataPacket.type = PACKET_DISCONNECT;
    clientDataPacket.clientId = clientId;

    ENetPacket* packet = enet_packet_create(&clientDataPacket, sizeof(ClientDataPacket), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(server, 0, packet);
}

void Server::PacketReceived(ENetEvent enetEvent)
{
    PacketType* packetType = (PacketType*)enetEvent.packet->data;

    if (*packetType == PACKET_INPUT)
    {
        InputPacket* inputPacket = (InputPacket*)enetEvent.packet->data;
        int entityId = inputPacket->clientId;

        PeerEntity entity = peerEntities[entityId];
        entity.entity->SetInputs(inputPacket->up, inputPacket->down, inputPacket->left, inputPacket->right);
    }
    if (*packetType == PACKET_CONNECT)
    {
        ClientDataPacket clientDataPacket;
        clientDataPacket.type = PACKET_CLIENT_DATA;

        int clientId;
        bool invalidId = true;
        while (invalidId)
        {
            std::srand(std::time({}));
            clientId = std::rand();
            if (!peerEntities.contains(clientId))
            {
                invalidId = false;
            }
        }

        peers[clientId] = enetEvent.peer;
        clientDataPacket.clientId = clientId;

        SDL_Log("New Client was given id: %d", clientDataPacket.clientId);

        ENetPacket* packet = enet_packet_create(&clientDataPacket, sizeof(ClientDataPacket), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(enetEvent.peer, 0, packet);

        for (auto kvp: peerEntities)
        {
            PeerEntity peerEntity = kvp.second;
            int peerId = kvp.first;

            StatePacket statePacket;
            statePacket.type = PACKET_CREATE_CLIENT_ENTITY;
            statePacket.clientId = peerId;
            statePacket.x = peerEntity.entity->GetPosition().x;
            statePacket.y = peerEntity.entity->GetPosition().y;

            ENetPacket* peerPacket = enet_packet_create(&statePacket, sizeof(StatePacket), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(enetEvent.peer, 0, peerPacket);
        }
    }
    if (*packetType == PACKET_DISCONNECT)
    {
        DisconnectPacket* disconnectPacket = (DisconnectPacket*)enetEvent.packet->data;
        int clientId = disconnectPacket->clientId;
        SDL_Log("Peer disconnected: %d", clientId);

        PeerEntity entity = peerEntities[clientId];
        delete entity.entity;
        peerEntities.erase(clientId);
        peers.erase(clientId);

        ClientDataPacket clientDataPacket;
        clientDataPacket.type = PACKET_DISCONNECT;
        clientDataPacket.clientId = clientId;

        ENetPacket* packet = enet_packet_create(&clientDataPacket, sizeof(ClientDataPacket), ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(server, 0, packet);
    }
    if (*packetType == PACKET_CREATE_CLIENT_ENTITY)
    {
        ClientDataPacket* clientDataPacket = (ClientDataPacket*)enetEvent.packet->data;
        ENetPeer* peer = enetEvent.peer;
        SDL_Log("Server entity created for: %d", clientDataPacket->clientId);

        ServerEntity* newEntity = new ServerEntity(64, 64);
        PeerEntity peerEntity;
        peerEntity.entity = newEntity;
        peerEntity.peer = peer;

        peerEntities[clientDataPacket->clientId] = peerEntity;

        StatePacket statePacket;
        statePacket.type = PACKET_CREATE_CLIENT_ENTITY;
        statePacket.clientId = clientDataPacket->clientId;
        statePacket.x = peerEntity.entity->GetPosition().x;
        statePacket.y = peerEntity.entity->GetPosition().y;

        ENetPacket* packet = enet_packet_create(&statePacket, sizeof(StatePacket), ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(server, 0, packet);
    }

    enet_packet_destroy(enetEvent.packet);
}

void Server::EnetEvent(ENetEvent enetEvent)
{
    switch (enetEvent.type)
    {
        case ENET_EVENT_TYPE_RECEIVE:
        {
            PacketReceived(enetEvent);
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
            for (auto kvp: std::map(peers))
            {
                ENetPeer* peer = kvp.second;
                int clientId = kvp.first;
                if (peer == enetEvent.peer)
                {
                    if (peers.contains(clientId))
                    {
                        peers.erase(clientId);
                    }
                    if (peerEntities.contains(clientId))
                    {
                        PeerEntity entity = peerEntities[clientId];
                        delete entity.entity;
                        peerEntities.erase(clientId);
                    }
                    ClientDataPacket clientDataPacket;
                    clientDataPacket.type = PACKET_DISCONNECT;
                    clientDataPacket.clientId = clientId;

                    ENetPacket* packet = enet_packet_create(&clientDataPacket, sizeof(ClientDataPacket), ENET_PACKET_FLAG_RELIABLE);
                    enet_host_broadcast(server, 0, packet);
                    break;
                }
            }

        default:
            break;
    }
}

void Server::Run()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        Event(event);
    }

    ENetEvent enetEvent;
    while (enet_host_service(server, &enetEvent, 0) > 0)
    {
        EnetEvent(enetEvent);
    }

    state->lastTick = state->currentTick;
    state->currentTick = SDL_GetTicks();
    state->deltaTime = (float)(state->currentTick - state->lastTick) / 1000.0f;

    for (auto& kvp : peerEntities)
    {
        PeerEntity entity = kvp.second;
        Position oldPos = entity.entity->GetPosition();
        entity.entity->Update(state->deltaTime);
        Position newPos = entity.entity->GetPosition();

        if (oldPos.x != newPos.x || oldPos.y != newPos.y)
        {
            int clientId = kvp.first;
            BroadcastEntityState(entity.entity, clientId);
        }
    }
}

void Server::Quit()
{
    if (terminalThread.joinable())
        terminalThread.join();

    for (auto& kvp : peerEntities)
    {
        PeerEntity entity = kvp.second;
        delete entity.entity;
    }

    SDL_DestroyWindow(state->window);
    delete(state);

    enet_host_destroy(server);
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

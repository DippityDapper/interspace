#include "World.h"

SDL_AppResult World::Init(Vec2 _worldSize)
{
    worldSize.x = _worldSize.x;
    worldSize.y = _worldSize.y;

    for (int y = 0; y < worldSize.y; ++y)
    {
        for (int x = 0; x < worldSize.x; ++x)
        {
            Vec2 gridPosition{x, y};
            Area area{};

            if (area.Init() == SDL_APP_FAILURE)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create world");
                return SDL_APP_FAILURE;
            }
            areas[gridPosition] = area;
        }
    }

    astar.SetHeuristic(AStar::MANHATTAN);

    NetworkManager::AddHandler(PACKET_POSITION, [this](ENetEvent& event) { HandlePositionPacket(event); });

    NetworkManager::connectionEvent.emplace_back([this](int clientId, NetworkManager* nm) { OnClientConnected(clientId, nm); });
    NetworkManager::disconnectionEvent.emplace_back([this](int clientId, NetworkManager* nm) { OnClientDisconnected(clientId); });

    return SDL_APP_SUCCESS;
}

void World::Update(float delta)
{
    for (auto& kvp : entities)
    {
        Vec2 oldPos = kvp.second->GetPosition();
        kvp.second->Update(delta);
        Vec2 newPos = kvp.second->GetPosition();

        if (oldPos.x != newPos.x || oldPos.y != newPos.y)
        {
            int clientId = kvp.first;
            entityPositionUpdated.push(clientId);
        }
    }

    GetCurrentArea()->Update(delta);
}

void World::NetworkUpdate(NetworkManager* nm)
{
    int positionQueueSize = entityPositionUpdated.size();
    for (int i = 0; i < positionQueueSize; ++i)
    {
        int clientId = entityPositionUpdated.front();
        entityPositionUpdated.pop();

        PositionPacket positionPacket;
        positionPacket.type = PACKET_POSITION;
        positionPacket.clientId = clientId;
        Vec2 pos = entities[clientId]->GetPosition();
        positionPacket.x = pos.x;
        positionPacket.y = pos.y;

        ENetPacket* packet = enet_packet_create(&positionPacket, sizeof(PositionPacket), ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
        enet_host_broadcast(nm->server, 0, packet);
    }
}

Area* World::GetCurrentArea()
{
    if (!areas.contains(currentAreaPosition))
        return nullptr;
    return &areas[currentAreaPosition];
}

void World::CleanEntities()
{
    entities.clear();
}

void World::OnClientConnected(int clientId, NetworkManager* nm)
{
    ENetPeer* peer = nm->peers[clientId];

    for (auto& kvp: entities)
    {
        int peerId = kvp.first;

        PositionPacket positionPacket;
        positionPacket.type = PACKET_CREATE_CLIENT_ENTITY;
        positionPacket.clientId = peerId;
        positionPacket.x = kvp.second->GetPosition().x;
        positionPacket.y = kvp.second->GetPosition().y;

        ENetPacket *peerPacket = enet_packet_create(&positionPacket, sizeof(PositionPacket), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, peerPacket);
    }

    float x = 320;
    float y = 180;
    entities[clientId] = std::make_unique<ServerEntity>(x, y);
    SDL_Log("Server entity created for: %d", clientId);

    PositionPacket positionPacket;
    positionPacket.type = PACKET_CREATE_CLIENT_ENTITY;
    positionPacket.clientId = clientId;
    positionPacket.x = x;
    positionPacket.y = y;

    ENetPacket* packet = enet_packet_create(&positionPacket, sizeof(PositionPacket), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(nm->server, 0, packet);
}

void World::OnClientDisconnected(int clientId)
{
    if (entities.contains(clientId))
    {
        entities.erase(clientId);
    }
}

void World::HandlePositionPacket(ENetEvent &enetEvent)
{
    PositionPacket* positionPacket = (PositionPacket*)enetEvent.packet->data;
    ServerEntity* entity = entities[positionPacket->clientId].get();

    Vec2 startPosition{entity->GetPosition()};
    Vec2 goalPosition{positionPacket->x, positionPacket->y};

    Area* currentArea = GetCurrentArea();
    Vec2 startGridPosition{currentArea->grid.GlobalToLocal(startPosition)};
    Vec2 goalGridPosition{currentArea->grid.GlobalToLocal(goalPosition)};

    std::vector<Vec2> path = astar.FindPath(startGridPosition, goalGridPosition, currentArea->grid);
    entity->SetPath(path);
}

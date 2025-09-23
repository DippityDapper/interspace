#include "server/world/World.h"

namespace Game
{
    void World::Init(Engine::Vec2<int> _worldSize)
    {
        worldSize.x = _worldSize.x;
        worldSize.y = _worldSize.y;

        for (int y = 0; y < worldSize.y; ++y)
        {
            for (int x = 0; x < worldSize.x; ++x)
            {
                Engine::Vec2<int> gridPosition{x, y};
                Area area{};

                area.Init();
                areas[gridPosition] = area;
            }
        }

        astar.SetHeuristic(Engine::MANHATTAN);

        Engine::NetworkManager::AddHandler(Engine::PACKET_POSITION, [this](ENetEvent& event) { HandlePositionPacket(event); });

        Engine::NetworkManager::connectionEvent.emplace_back([this](int clientId, Engine::NetworkManager* nm) { OnClientConnected(clientId, nm); });
        Engine::NetworkManager::disconnectionEvent.emplace_back([this](int clientId, Engine::NetworkManager* nm) { OnClientDisconnected(clientId); });
    }

    void World::Update(float delta)
    {
        for (auto& kvp : entities)
        {
            Engine::Vec2<float> oldPos = kvp.second->GetPosition();
            kvp.second->Update(delta);
            Engine::Vec2<float> newPos = kvp.second->GetPosition();

            if (oldPos.x != newPos.x || oldPos.y != newPos.y)
            {
                int clientId = kvp.first;
                entityPositionUpdated.push(clientId);
            }
        }

        GetCurrentArea()->Update(delta);
    }

    void World::NetworkUpdate(Engine::NetworkManager* nm)
    {
        int positionQueueSize = entityPositionUpdated.size();
        for (int i = 0; i < positionQueueSize; ++i)
        {
            int clientId = entityPositionUpdated.front();
            entityPositionUpdated.pop();

            Engine::PositionPacket positionPacket;
            positionPacket.type = Engine::PACKET_POSITION;
            positionPacket.clientId = clientId;
            Engine::Vec2<float> pos = entities[clientId]->GetPosition();
            positionPacket.x = pos.x;
            positionPacket.y = pos.y;

            ENetPacket* packet = enet_packet_create(&positionPacket, sizeof(Engine::PositionPacket), ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
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

    void World::OnClientConnected(int clientId, Engine::NetworkManager* nm)
    {
        ENetPeer* peer = nm->peers[clientId];

        for (auto& kvp: entities)
        {
            int peerId = kvp.first;

            Engine::CreateEntityPacket entityPacket;
            entityPacket.type = Engine::PACKET_CREATE_CLIENT_ENTITY;
            entityPacket.clientId = peerId;
            strcpy(entityPacket.userName, Game::GameData::GetUser(peerId).c_str());
            entityPacket.x = kvp.second->GetPosition().x;
            entityPacket.y = kvp.second->GetPosition().y;

            ENetPacket *peerPacket = enet_packet_create(&entityPacket, sizeof(Engine::CreateEntityPacket), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, peerPacket);
        }

        float x = 320;
        float y = 180;
        Engine::Vec2<float> pos{x, y};
        if (!Game::GameData::GetCharacterPosition(clientId, pos))
        {
            GameData::AddOrUpdateCharacter(clientId, x, y);
        }

        entities[clientId] = std::make_unique<ServerEntity>(pos.x, pos.y);
        SDL_Log("Server entity created for: %d", clientId);

        Engine::CreateEntityPacket entityPacket;
        entityPacket.type = Engine::PACKET_CREATE_CLIENT_ENTITY;
        entityPacket.clientId = clientId;
        strcpy(entityPacket.userName, Game::GameData::GetUser(clientId).c_str());
        entityPacket.x = pos.x;
        entityPacket.y = pos.y;

        ENetPacket *packet = enet_packet_create(&entityPacket, sizeof(Engine::CreateEntityPacket), ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(nm->server, 0, packet);
    }

    void World::OnClientDisconnected(int clientId)
    {
        if (entities.contains(clientId))
        {
            GameData::AddOrUpdateCharacter(clientId, entities[clientId]->GetPosition().x, entities[clientId]->GetPosition().y);
            entities.erase(clientId);
        }
    }

    void World::HandlePositionPacket(ENetEvent &enetEvent)
    {
        Engine::PositionPacket* positionPacket = (Engine::PositionPacket*)enetEvent.packet->data;
        ServerEntity* entity = entities[positionPacket->clientId].get();

        Engine::Vec2<float> startPosition{entity->GetPosition()};
        Engine::Vec2<float> goalPosition{positionPacket->x, positionPacket->y};

        Area* currentArea = GetCurrentArea();
        Engine::Vec2 startGridPosition{currentArea->grid.GlobalToLocal(startPosition)};
        Engine::Vec2 goalGridPosition{currentArea->grid.GlobalToLocal(goalPosition)};

        std::vector<Engine::Vec2<int>> path = astar.FindPath(startGridPosition, goalGridPosition, currentArea->grid);
        entity->SetPath(path);
    }
}

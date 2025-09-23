#include "client/world/World.h"

namespace Game
{
    SDL_AppResult World::Init(Engine::Vec2<int> _worldSize, SDL_Renderer* _renderer)
    {
        renderer = _renderer;

        worldSize.x = _worldSize.x;
        worldSize.y = _worldSize.y;

        for (int y = 0; y < worldSize.y; ++y)
        {
            for (int x = 0; x < worldSize.x; ++x)
            {
                Engine::Vec2<int> gridPosition{x, y};
                Area area{};

                area.Init(_renderer);
                areas[gridPosition] = area;
            }
        }

        Engine::NetworkManager::AddHandler(Engine::PACKET_POSITION, [this](ENetEvent& event)
        {
            HandlePositionPacket(event);
        });
        Engine::NetworkManager::AddHandler(Engine::PACKET_CREATE_CLIENT_ENTITY, [this](ENetEvent& event)
        {
            HandleCreateClientEntityPacket(event);
        });
        Engine::NetworkManager::disconnectionEvent.emplace_back([this](int clientId, Engine::NetworkManager* nm)
        {
            OnClientDisconnect(clientId);
        });

        return SDL_APP_SUCCESS;
    }

    Area* World::GetCurrentArea()
    {
        if (!areas.contains(currentAreaPosition))
            return nullptr;
        return &areas[currentAreaPosition];
    }

    void World::Render(SDL_Renderer *renderer, Engine::Camera& camera)
    {
        Area* currentArea = GetCurrentArea();
        currentArea->RenderTiles(renderer, camera);
        RenderEntities(renderer, camera);
    }

    void World::RenderEntities(SDL_Renderer *renderer, Engine::Camera& camera)
    {
        for (auto& kvp : entities)
        {
            kvp.second->Render(renderer, camera);
        }
    }

    void World::CleanEntities()
    {
        entities.clear();
    }

    void World::HandlePositionPacket(ENetEvent& event)
    {
        Engine::PositionPacket* pPositionPacket = (Engine::PositionPacket*)event.packet->data;
        int clientId = pPositionPacket->clientId;

        entities[clientId]->SetPosition(pPositionPacket->x, pPositionPacket->y);
    }

    void World::OnClientDisconnect(int clientId)
    {
        entities.erase(clientId);
    }

    void World::HandleCreateClientEntityPacket(ENetEvent& event)
    {
        Engine::PositionPacket* pPositionPacket = (Engine::PositionPacket*)event.packet->data;
        SDL_Log("Client entity created for: %d", pPositionPacket->clientId);

        std::string texturePath = "player/crew_1.png";
        entities[pPositionPacket->clientId] = std::make_unique<ClientEntity>(renderer, texturePath, pPositionPacket->x, pPositionPacket->y);
    }
}

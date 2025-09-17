#include "World.h"
#include "../../common/src/Packets.h"
#include "Game.h"
#include "../../server/src/World.h"


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

    NetworkManager::AddHandler(PACKET_POSITION, [this](ENetEvent& event) { HandlePositionPacket(event); });
    NetworkManager::AddHandler(PACKET_CREATE_CLIENT_ENTITY, [this](ENetEvent& event) { HandleCreateClientEntityPacket(event); });

    NetworkManager::disconnectionEvent.emplace_back([this](int clientId, NetworkManager* nm) { OnClientDisconnect(clientId); });

    return SDL_APP_SUCCESS;
}

Area* World::GetCurrentArea()
{
    if (!areas.contains(currentAreaPosition))
        return nullptr;
    return &areas[currentAreaPosition];
}

void World::Render(SDL_Renderer *renderer, Camera& camera)
{
    Area* currentArea = GetCurrentArea();
    currentArea->RenderTiles(renderer, camera);
    RenderEntities(renderer, camera);
}

void World::RenderEntities(SDL_Renderer *renderer, Camera& camera)
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
    PositionPacket* pPositionPacket = (PositionPacket*)event.packet->data;
    int clientId = pPositionPacket->clientId;

    entities[clientId]->SetPosition(pPositionPacket->x, pPositionPacket->y);
}

void World::OnClientDisconnect(int clientId)
{
    entities.erase(clientId);
}

void World::HandleCreateClientEntityPacket(ENetEvent& event)
{
    PositionPacket* pPositionPacket = (PositionPacket*)event.packet->data;
    SDL_Log("Client entity created for: %d", pPositionPacket->clientId);

    entities[pPositionPacket->clientId] = std::make_unique<ClientEntity>(Game::state->renderer, "player/crew_1.png", pPositionPacket->x, pPositionPacket->y);
}

#include "World.h"
#include "../../common/Packets.h"
#include "Game.h"

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

    NetworkManager::packetHandlers[PACKET_DISCONNECT] = [this](ENetEvent& event) { HandleDisconnectPacket(event); };
    NetworkManager::packetHandlers[PACKET_STATE] = [this](ENetEvent& event) { HandleStatePacket(event); };
    NetworkManager::packetHandlers[PACKET_CREATE_CLIENT_ENTITY] = [this](ENetEvent& event) { HandleCreateClientEntityPacket(event); };

    return SDL_APP_SUCCESS;
}

Area *World::GetCurrentArea()
{
    if (!areas.contains(currentAreaPosition))
        return nullptr;
    return &areas[currentAreaPosition];
}

void World::Render(SDL_Renderer *renderer)
{
    Area* currentArea = GetCurrentArea();
    currentArea->RenderTiles(renderer);
    RenderEntities(renderer);
}

void World::RenderEntities(SDL_Renderer *renderer)
{
    for (auto& kvp : entities)
    {
        kvp.second->Render(renderer);
    }
}

void World::CleanEntities()
{
    entities.clear();
}

void World::HandleStatePacket(ENetEvent& event)
{
    StatePacket* statePacket = (StatePacket*)event.packet->data;
    int clientId = statePacket->clientId;

    entities[clientId]->SetPosition(statePacket->x, statePacket->y);
}

void World::HandleDisconnectPacket(ENetEvent& event)
{
    DisconnectPacket* disconnectPacket = (DisconnectPacket*)event.packet->data;
    int clientId = disconnectPacket->clientId;
    SDL_Log("Peer disconnected: %d", clientId);

    entities.erase(clientId);
}

void World::HandleCreateClientEntityPacket(ENetEvent& event)
{
    StatePacket* statePacket = (StatePacket*)event.packet->data;
    SDL_Log("Client entity created for: %d", statePacket->clientId);

    entities[statePacket->clientId] = std::make_unique<ClientEntity>(Game::state->renderer, "player/crew_1.png", statePacket->x, statePacket->y);
}

#include "interspace/client/World.hpp"

#include <ranges>

#include "SDL3/SDL_log.h"

#include "igneous/input/Input.hpp"
#include "igneous/networking/Serializer.hpp"

#include "interspace/client/Tiles.hpp"
#include "interspace/menus/CreateFactionMenu.hpp"
#include "interspace/network/NetworkPackets.hpp"

namespace Interspace::Client
{
    void World::Init()
    {
    }

    void World::InitWorld(Client* _client)
    {
        client = _client;

        worldData = std::make_unique<WorldData>();
        RegisterNetEvents();
        Tiles::Init();
    }

    void World::Update(float delta)
    {
        for (const auto& faction : factions | std::views::values)
        {
            faction->Update(delta);
        }

        if (chunkGenerationTimer < chunkGenerationClock)
            chunkGenerationTimer += delta;
        else
        {
            chunkGenerationTimer -= chunkGenerationClock;
            GenerateChunks();
        }
    }

    void World::Render()
    {
        for (const auto& faction : factions | std::views::values)
        {
            for (const auto& colonist : faction->colonists | std::views::values)
            {
                colonist->RenderName(faction->data.name);
            }
        }
    }

    void World::HandleEvents(Engine::InputLayer& layer)
    {
        if (layer.Is("gameplay"))
        {
            if (Engine::Input::IsKeyJustPressed(SDLK_ESCAPE))
            {
                if (!disconnectRequested && client)
                {
                    std::vector<uint8_t> request{DISCONNECTION_REQUEST};
                    Engine::Serializer serializer(request);

                    serializer << client->clientId;

                    client->netInterface->SendToServer(request, ENET_PACKET_FLAG_RELIABLE);
                    disconnectRequested = true;
                }
            }
            if (Engine::Input::IsKeyJustPressed(SDLK_Q))
            {
                Faction* myFaction = nullptr;
                for (const auto& faction : factions | std::views::values)
                {
                    if (faction->data.ownerId == client->clientId)
                    {
                        myFaction = faction.get();
                        break;
                    }
                }
                if (myFaction)
                {
                    std::string colonistName{"Dwayne"};
                    std::vector<uint8_t> data{CREATE_COLONIST_REQUEST};

                    Engine::Serializer serializer(data);
                    serializer << myFaction->data.id << colonistName;

                    client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
                }
            }
            if (Engine::Input::IsButtonJustPressed(SDL_BUTTON_LEFT))
            {
                Faction* colonistFaction = nullptr;
                Colonist* selectedColonist = nullptr;
                bool hasSelectedColonist = false;
                for (const auto& faction : factions | std::views::values)
                {
                    if (!faction->data.members.contains(client->clientId))
                        continue;

                    for (const auto& colonist : faction->colonists | std::views::values)
                    {
                        if (colonist->sprite->IsMouseWithin())
                        {
                            colonistFaction = faction.get();
                            selectedColonist = colonist.get();
                            break;
                        }
                        if (colonist->colonistData.selectedBy == client->clientId)
                        {
                            hasSelectedColonist = true;
                        }
                    }
                    if (selectedColonist)
                        break;
                }
                if (selectedColonist && colonistFaction)
                {
                    if (selectedColonist->colonistData.selectedBy != client->clientId)
                    {
                        std::vector<uint8_t> data{COLONIST_SELECT_REQUEST};

                        Engine::Serializer serializer(data);
                        serializer << colonistFaction->data.id
                                   << selectedColonist->entityData.id << client->clientId;

                        client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
                    }
                    else
                    {
                        std::vector<uint8_t> data{COLONIST_DESELECT_REQUEST};

                        Engine::Serializer serializer(data);
                        serializer << colonistFaction->data.id
                                   << selectedColonist->entityData.id << client->clientId;

                        client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
                    }
                }
                else if (hasSelectedColonist)
                {
                    std::vector<uint8_t> data{COLONIST_DESELECT_ALL_REQUEST};

                    Engine::Serializer serializer(data);
                    serializer << client->clientId;
                    client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
                }
            }
            if (Engine::Input::IsButtonJustPressed(SDL_BUTTON_RIGHT))
            {
                for (const auto& faction : factions | std::views::values)
                {
                    for (const auto& colonist : faction->colonists | std::views::values)
                    {
                        if (colonist->colonistData.selectedBy != client->clientId)
                            continue;

                        std::vector<uint8_t> data{COLONIST_POSITION_REQUEST};

                        Engine::Vec2<float> mousePosition = camera->GetMouseGlobalPosition();
                        float posX = mousePosition.x;
                        float posY = mousePosition.y;

                        Engine::Serializer serializer(data);
                        serializer << faction->data.id << colonist->entityData.id << posX
                                   << posY;

                        client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
                    }
                }
            }
        }
    }

    void World::Clean()
    {
    }

    void World::GenerateChunks()
    {
        uint16_t maxChunkPerCycle = 4;
        uint16_t maxChunkIndex = 0;

        for (int i = 0; i < chunkDataQueue.size(); i++)
        {
            std::vector<uint8_t>& data = chunkDataQueue.front();

            uint16_t chunkX = 0;

            uint16_t chunkY = 0;

            Engine::Deserializer deserializer(data);
            deserializer >> chunkX >> chunkY;

            Engine::Vec2<uint16_t> chunkPos{chunkX, chunkY};
            if (!chunks.contains(chunkPos))
                return;

            Chunk* chunk = chunks[chunkPos].get();

            chunk->BeginTileUpdate();
            for (uint16_t w = chunk->tiles.size();
                 w < worldData->CHUNK_SIZE * worldData->CHUNK_SIZE; w++)
            {
                uint8_t tileX = w % worldData->CHUNK_SIZE;
                uint8_t tileY = w / worldData->CHUNK_SIZE;

                uint32_t tileId = 0;
                uint32_t tileVariant = 0;

                deserializer >> tileId >> tileVariant;

                Engine::Vec2<uint8_t> tilePos{tileX, tileY};
                chunk->tiles.emplace(tilePos, Tiles::GetTileOfType(tileId, tileVariant));
                Tile* tile = chunk->tiles[tilePos];

                chunk->UpdateTile(tilePos, tile);
            }
            chunk->EndTileUpdate();

            if (chunk->tiles.size() >= worldData->CHUNK_SIZE * worldData->CHUNK_SIZE)
            {
                chunkDataQueue.pop();
                SDL_Log("[Client] Chunk finished at (%u, %u).", chunk->data.position.x,
                        chunk->data.position.y);
            }

            maxChunkIndex++;
            if (maxChunkIndex > maxChunkPerCycle)
                break;
        }
    }
}
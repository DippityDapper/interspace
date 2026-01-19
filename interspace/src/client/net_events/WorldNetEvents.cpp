#include <random>

#include "interspace/client/Tiles.hpp"
#include "interspace/client/World.hpp"
#include "../../../../../igneous/include/igneous/networking/Serializer.hpp"
#include "SDL3/SDL_log.h"

namespace Interspace::Client
{
    void World::OnWorldDataReceived(const std::vector<uint8_t>& data)
    {
        Engine::Deserializer deserializer(data);
        deserializer >> worldData->worldSizeX >> worldData->worldSizeY >> worldName;

        std::mt19937 gen(std::random_device{}());

        float posX = 0;
        std::uniform_int_distribution<> posXDist(0, worldData->worldSizeX * worldData->CHUNK_SIZE * worldData->TILE_SIZE);
        posX = (float) posXDist(gen);

        float posY = 0;
        std::uniform_int_distribution<> posYDist(0, worldData->worldSizeY * worldData->CHUNK_SIZE * worldData->TILE_SIZE);
        posY = (float) posYDist(gen);

        camera = std::make_unique<Camera>(
                posX,
                posY,
                1.0f);
        camera->SetCurrent();

        camera->minZoom = 0.05;
        camera->limitBounds = true;
        camera->limitLeft = 0.0f;
        camera->limitRight = worldData->worldSizeX * worldData->CHUNK_SIZE * worldData->TILE_SIZE;
        camera->limitTop = 0.0f;
        camera->limitBottom = worldData->worldSizeX * worldData->CHUNK_SIZE * worldData->TILE_SIZE;
    }

    void World::OnGeneratedChunkReceived(const std::vector<uint8_t>& data)
    {
        uint16_t chunkX = 0;
        uint16_t chunkY = 0;

        Engine::Deserializer deserializer(data);
        deserializer >> chunkX >> chunkY;

        Engine::Vec2<uint16_t> chunkPos{chunkX, chunkY};
        if (chunks.contains(chunkPos))
            return;

        chunks.emplace(chunkPos, std::make_unique<Chunk>(chunkPos));

        Chunk* chunk = chunks[chunkPos].get();
        chunkDataQueue.push(data);
        chunk->data.position = chunkPos;

        SDL_Log("[Client] Chunk received (%u, %u).", chunkX, chunkY);
    }
}

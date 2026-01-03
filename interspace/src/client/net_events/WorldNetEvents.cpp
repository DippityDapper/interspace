#include <random>

#include "interspace/client/World.hpp"
#include "interspace/network/Serializer.hpp"

namespace Interspace::Client
{
    void World::OnWorldDataReceived(const std::vector<uint8_t>& data)
    {
        Deserializer deserializer(data);
        deserializer
            >> worldData->worldSizeX
            >> worldData->worldSizeY
            >> worldName;

        std::mt19937 gen(std::random_device{}());

        float posX = 0;
        std::uniform_int_distribution<> posXDist(0, worldData->worldSizeX * worldData->CHUNK_SIZE * worldData->TILE_SIZE);
        posX = (float)posXDist(gen);

        float posY = 0;
        std::uniform_int_distribution<> posYDist(0, worldData->worldSizeY * worldData->CHUNK_SIZE * worldData->TILE_SIZE);
        posY = (float)posYDist(gen);

        camera = std::make_unique<Camera>
        (
            posX,
            posY,
            1.0f
        );
        camera->SetCurrent();

        camera->minZoom = 0.05;
        camera->limitBounds = true;
        camera->limitLeft = 0.0f;
        camera->limitRight = worldData->worldSizeX * worldData->CHUNK_SIZE * worldData->TILE_SIZE;
        camera->limitTop = 0.0f;
        camera->limitBottom = worldData->worldSizeX * worldData->CHUNK_SIZE * worldData->TILE_SIZE;
    }
}

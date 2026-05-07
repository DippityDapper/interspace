#include "interspace/shared/world/generators/TestGenerator.hpp"

#include "interspace/shared/datahelpers/TileManager.hpp"
#include "interspace/shared/world/WorldData.hpp"

#include <chrono>

namespace Interspace
{
    void TestGenerator::Generate(ChunkData* chunk)
    {
        for (uint16_t w = 0; w < WorldData::CHUNK_SIZE * WorldData::CHUNK_SIZE; w++)
        {
            uint8_t tileX = w % WorldData::CHUNK_SIZE;
            uint8_t tileY = w / WorldData::CHUNK_SIZE;
            Engine::Vec2<uint8_t> tilePosition{tileX, tileY};
            chunk->tiles.emplace(tilePosition, TileManager::GetRandomTileBySeed("grass_flower", 1, chunk->position.x, chunk->position.y));
        }

        chunk->lastModified = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        chunk->isModified = false;
    }
}

#include "interspace/server/ServerOverworldGenerator.hpp"

#include "interspace/game/Game.hpp"

namespace Interspace::Server
{
    void ServerOverworldGenerator::GenerateChunk(ServerChunk* chunk)
    {
        for (uint16_t w = 0; w < Game::serverWorld->worldData->CHUNK_SIZE * Game::serverWorld->worldData->CHUNK_SIZE; w++)
        {
            uint8_t tileX = w % Game::serverWorld->worldData->CHUNK_SIZE;
            uint8_t tileY = w / Game::serverWorld->worldData->CHUNK_SIZE;
            Engine::Vec2<uint8_t> tilePosition{tileX, tileY};
            chunk->tiles.emplace(tilePosition, Game::serverWorld->tileRegistry->GetRandomTileBySeed("grass_flower", chunk->tileGen));
        }

        chunk->lastModified = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        chunk->isModified = false;
        SaveChunk(chunk);
    }
}
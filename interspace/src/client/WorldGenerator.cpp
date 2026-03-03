#include "interspace/client/WorldGenerator.hpp"
#include "interspace/client/World.hpp"
#include "interspace/client/Tiles.hpp"
#include "interspace/game/Game.hpp"

#include "SDL3/SDL_log.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"

#include <fstream>
#include <filesystem>
#include <chrono>
#include <ranges>

namespace Interspace::Client
{
    uint32_t WorldGenerator::CalculateBytesPerChunk() const
    {
        uint32_t tilesDataSize = World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE * (sizeof(uint32_t) * 2);
        return sizeof(bool) + sizeof(uint64_t) + tilesDataSize;
    }

    std::string WorldGenerator::GetRegionPath(uint16_t regionX, uint16_t regionY) const
    {
        return "data/client/" + Game::clientWorld->worldName + "/regions/" + std::to_string(regionX) + "_" + std::to_string(regionY) + ".region";
    }

    void WorldGenerator::GetRegionCoordinates(const Engine::Vec2<uint16_t>& chunkPosition, uint16_t& regionX, uint16_t& regionY, uint16_t& regionXIndex, uint16_t& regionYIndex) const
    {
        regionX = chunkPosition.x / World::worldData->REGION_SIZE;
        regionY = chunkPosition.y / World::worldData->REGION_SIZE;
        regionXIndex = chunkPosition.x % World::worldData->REGION_SIZE;
        regionYIndex = chunkPosition.y % World::worldData->REGION_SIZE;
    }

    void WorldGenerator::Tick()
    {
        uint16_t playerChunkX = Engine::Camera::main->position.x / (static_cast<float>(World::worldData->CHUNK_SIZE) * static_cast<float>(World::worldData->TILE_SIZE));
        uint16_t playerChunkY = Engine::Camera::main->position.y / (static_cast<float>(World::worldData->CHUNK_SIZE) * static_cast<float>(World::worldData->TILE_SIZE));

        TryLoadChunks({playerChunkX, playerChunkY}, 3);
        UnloadDistantChunks({playerChunkX, playerChunkY}, 3);

        std::set<Engine::Vec2<uint16_t>> visibleChunks{};
    }

    void WorldGenerator::QueueChunk(const Engine::Vec2<uint16_t>& chunkPosition)
    {
        if (chunks.contains(chunkPosition))
            return;

        chunks.emplace(chunkPosition, std::make_unique<Chunk>(chunkPosition));
        Chunk* chunk = chunks[chunkPosition].get();
        chunk->data.position = chunkPosition;
        chunk->data.lastModified = 0;

        if (LoadChunkFromCache(chunk))
        {
            RequestChunkNatural(chunk);
            SDL_Log("[Client] Chunk loaded from cache at (%u, %u) with timestamp %lu.", chunkPosition.x, chunkPosition.y, chunk->data.lastModified);
        }
        else
        {
            RequestChunkNatural(chunk);
            SDL_Log("[Client] Chunk not in cache at (%u, %u). Will request from server.", chunkPosition.x, chunkPosition.y);
        }
    }

    bool WorldGenerator::LoadChunkFromCache(Chunk* chunk)
    {
        uint16_t regionX, regionY, regionXIndex, regionYIndex;
        GetRegionCoordinates(chunk->data.position, regionX, regionY, regionXIndex, regionYIndex);
        std::string regionPath = GetRegionPath(regionX, regionY);

        if (!std::filesystem::exists(regionPath))
            return false;

        std::ifstream regionFile{regionPath, std::ios::binary};
        if (!regionFile.is_open())
            return false;
        uint32_t bytesPerChunk = CalculateBytesPerChunk();
        uint32_t chunkIndexInRegion = regionYIndex * World::worldData->REGION_SIZE + regionXIndex;
        uint32_t cursorPos = chunkIndexInRegion * bytesPerChunk;

        regionFile.seekg(0, std::ios::end);
        std::streampos fileSize = regionFile.tellg();
        uint32_t expectedFileSize = World::worldData->REGION_SIZE * World::worldData->REGION_SIZE * bytesPerChunk;

        if (fileSize < expectedFileSize)
        {
            regionFile.close();
            return false;
        }

        regionFile.seekg(cursorPos, std::ios::beg);

        bool chunkGenerated = false;
        regionFile.read(reinterpret_cast<char*>(&chunkGenerated), sizeof(bool));

        if (!chunkGenerated)
        {
            regionFile.close();
            return false;
        }

        uint64_t timestamp = 0;
        regionFile.read(reinterpret_cast<char*>(&timestamp), sizeof(uint64_t));
        chunk->data.lastModified = timestamp;

        chunk->BeginTileUpdate();
        for (int i = 0; i < World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE; i++)
        {
            uint32_t tileId = 0;
            regionFile.read(reinterpret_cast<char*>(&tileId), sizeof(uint32_t));
            uint32_t tileVariant = 0;
            regionFile.read(reinterpret_cast<char*>(&tileVariant), sizeof(uint32_t));

            uint8_t tileX = i % World::worldData->CHUNK_SIZE;
            uint8_t tileY = i / World::worldData->CHUNK_SIZE;
            Engine::Vec2<uint8_t> tilePosition{tileX, tileY};

            chunk->tiles.emplace(tilePosition, Tiles::GetTileOfType(tileId, tileVariant));
            chunk->UpdateTile(tilePosition, chunk->tiles[tilePosition]);
        }
        chunk->EndTileUpdate();

        regionFile.close();
        return true;
    }

    Chunk* WorldGenerator::GetChunk(const Engine::Vec2<uint16_t>& chunkPosition)
    {
        if (!chunks.contains(chunkPosition))
            return nullptr;

        Chunk* chunk = chunks[chunkPosition].get();

        if (chunk->tiles.size() < World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE)
            return nullptr;

        return chunk;
    }

    void WorldGenerator::SaveChunk(const Engine::Vec2<uint16_t>& chunkPosition)
    {
        if (!chunks.contains(chunkPosition))
            return;

        Chunk* chunk = chunks[chunkPosition].get();
        SaveChunk(chunk);
    }

    void WorldGenerator::SaveChunk(Chunk* chunk)
    {
        if (!chunk)
            return;

        if (chunk->tiles.size() < World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE)
            return;

        uint16_t regionX, regionY, regionXIndex, regionYIndex;
        GetRegionCoordinates(chunk->data.position, regionX, regionY, regionXIndex, regionYIndex);
        std::string regionPath = GetRegionPath(regionX, regionY);

        uint32_t bytesPerChunk = CalculateBytesPerChunk();
        uint32_t chunkIndexInRegion = regionYIndex * World::worldData->REGION_SIZE + regionXIndex;
        uint32_t cursorPos = chunkIndexInRegion * bytesPerChunk;

        if (std::filesystem::exists(regionPath))
        {
            std::fstream regionFile{regionPath, std::ios::binary | std::ios::in | std::ios::out};
            regionFile.seekp(cursorPos, std::ios::beg);

            bool chunkGenerated = true;
            regionFile.write(reinterpret_cast<const char*>(&chunkGenerated), sizeof(bool));

            uint64_t timestamp = chunk->data.lastModified;
            regionFile.write(reinterpret_cast<const char*>(&timestamp), sizeof(uint64_t));

            for (int w = 0; w < World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE; w++)
            {
                uint8_t tileX = w % World::worldData->CHUNK_SIZE;
                uint8_t tileY = w / World::worldData->CHUNK_SIZE;
                Engine::Vec2<uint8_t> tilePosition{tileX, tileY};

                if (chunk->tiles.contains(tilePosition))
                {
                    Tile* tile = chunk->tiles[tilePosition];
                    uint32_t tileId = tile->data.tileId;
                    uint32_t tileVariant = tile->data.variant;
                    regionFile.write(reinterpret_cast<const char*>(&tileId), sizeof(uint32_t));
                    regionFile.write(reinterpret_cast<const char*>(&tileVariant), sizeof(uint32_t));
                }
                else
                {
                    uint32_t tileId = 0;
                    uint32_t tileVariant = 0;
                    regionFile.write(reinterpret_cast<const char*>(&tileId), sizeof(uint32_t));
                    regionFile.write(reinterpret_cast<const char*>(&tileVariant), sizeof(uint32_t));
                }
            }
            regionFile.close();
        }
        else
        {
            std::filesystem::create_directories(std::filesystem::path(regionPath).parent_path());
            std::ofstream regionFile{regionPath, std::ios::binary};

            uint16_t chunkCount = World::worldData->REGION_SIZE * World::worldData->REGION_SIZE;
            for (uint16_t i = 0; i < chunkCount; i++)
            {
                uint16_t chunkXInRegion = i % World::worldData->REGION_SIZE;
                uint16_t chunkYInRegion = i / World::worldData->REGION_SIZE;
                Engine::Vec2<uint16_t> chunkPos{
                        static_cast<uint16_t>(regionX * World::worldData->REGION_SIZE + chunkXInRegion),
                        static_cast<uint16_t>(regionY * World::worldData->REGION_SIZE + chunkYInRegion)};

                Chunk* currentChunk = nullptr;
                if (chunks.contains(chunkPos))
                    currentChunk = chunks[chunkPos].get();

                bool chunkGenerated = currentChunk != nullptr && currentChunk->tiles.size() >= World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE;
                regionFile.write(reinterpret_cast<const char*>(&chunkGenerated), sizeof(bool));

                uint64_t timestamp = 0;
                if (currentChunk)
                    timestamp = currentChunk->data.lastModified;

                regionFile.write(reinterpret_cast<const char*>(&timestamp), sizeof(uint64_t));

                for (int w = 0; w < World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE; w++)
                {
                    uint8_t tileX = w % World::worldData->CHUNK_SIZE;
                    uint8_t tileY = w / World::worldData->CHUNK_SIZE;
                    Engine::Vec2<uint8_t> tilePosition{tileX, tileY};

                    if (currentChunk && currentChunk->tiles.contains(tilePosition))
                    {
                        Tile* tile = currentChunk->tiles[tilePosition];
                        uint32_t tileId = tile->data.tileId;
                        uint32_t tileVariant = tile->data.variant;
                        regionFile.write(reinterpret_cast<const char*>(&tileId), sizeof(uint32_t));
                        regionFile.write(reinterpret_cast<const char*>(&tileVariant), sizeof(uint32_t));
                    }
                    else
                    {
                        uint32_t tileId = 0;
                        uint32_t tileVariant = 0;
                        regionFile.write(reinterpret_cast<const char*>(&tileId), sizeof(uint32_t));
                        regionFile.write(reinterpret_cast<const char*>(&tileVariant), sizeof(uint32_t));
                    }
                }
            }
            regionFile.close();
        }
    }

    std::map<Engine::Vec2<uint16_t>, uint64_t> WorldGenerator::GetChunkTimestamps(const std::set<Engine::Vec2<uint16_t>>& chunkPositions)
    {
        std::map<Engine::Vec2<uint16_t>, uint64_t> timestamps;

        for (const auto& chunkPos: chunkPositions)
        {
            uint64_t timestamp = GetChunkTimestamp(chunkPos);
            timestamps[chunkPos] = timestamp;
        }

        return timestamps;
    }

    uint64_t WorldGenerator::GetChunkTimestamp(const Engine::Vec2<uint16_t>& chunkPosition)
    {
        if (chunks.contains(chunkPosition))
        {
            Chunk* chunk = chunks[chunkPosition].get();
            if (chunk->tiles.size() >= World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE)
            {
                return chunk->data.lastModified;
            }
        }

        uint16_t regionX, regionY, regionXIndex, regionYIndex;
        GetRegionCoordinates(chunkPosition, regionX, regionY, regionXIndex, regionYIndex);
        std::string regionPath = GetRegionPath(regionX, regionY);

        if (!std::filesystem::exists(regionPath))
            return 0;

        std::ifstream regionFile{regionPath, std::ios::binary};
        if (!regionFile.is_open())
            return 0;

        uint32_t bytesPerChunk = CalculateBytesPerChunk();
        uint32_t chunkIndexInRegion = regionYIndex * World::worldData->REGION_SIZE + regionXIndex;
        uint32_t cursorPos = chunkIndexInRegion * bytesPerChunk;

        regionFile.seekg(cursorPos, std::ios::beg);

        bool chunkGenerated = false;
        regionFile.read(reinterpret_cast<char*>(&chunkGenerated), sizeof(bool));

        if (!chunkGenerated)
        {
            regionFile.close();
            return 0;
        }

        uint64_t timestamp = 0;
        regionFile.read(reinterpret_cast<char*>(&timestamp), sizeof(uint64_t));
        regionFile.close();

        return timestamp;
    }

    void WorldGenerator::ReceiveChunkFromServer(const Engine::Vec2<uint16_t>& chunkPosition, uint64_t timestamp, const std::vector<std::pair<uint32_t, uint32_t>>& tileData)
    {
        if (!chunks.contains(chunkPosition))
        {
            chunks.emplace(chunkPosition, std::make_unique<Chunk>(chunkPosition));
        }

        Chunk* chunk = chunks[chunkPosition].get();
        chunk->data.position = chunkPosition;
        chunk->data.lastModified = timestamp;
        chunk->tiles.clear();

        if (tileData.size() != World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE)
        {
            SDL_Log("[Client] ERROR: Received chunk data size mismatch at (%u, %u). Expected %u, got %zu.",
                    chunkPosition.x,
                    chunkPosition.y,
                    World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE,
                    tileData.size());
            return;
        }

        chunk->BeginTileUpdate();
        for (int i = 0; i < tileData.size(); i++)
        {
            uint8_t tileX = i % World::worldData->CHUNK_SIZE;
            uint8_t tileY = i / World::worldData->CHUNK_SIZE;
            Engine::Vec2<uint8_t> tilePosition{tileX, tileY};

            uint32_t tileId = tileData[i].first;
            uint32_t tileVariant = tileData[i].second;

            chunk->tiles.emplace(tilePosition, Tiles::GetTileOfType(tileId, tileVariant));
            chunk->UpdateTile(tilePosition, chunk->tiles[tilePosition]);
        }
        chunk->EndTileUpdate();

        SaveChunk(chunk);
    }

    void WorldGenerator::TryLoadChunks(const Engine::Vec2<uint16_t>& playerChunkPosition, int viewDistance)
    {
        for (int y = playerChunkPosition.y - viewDistance; y <= playerChunkPosition.y + viewDistance; ++y)
        {
            for (int x = playerChunkPosition.x - viewDistance; x <= playerChunkPosition.x + viewDistance; ++x)
            {
                if (x < 0 || x > World::worldData->worldSizeX || y < 0 || y > World::worldData->worldSizeY)
                    continue;

                Engine::Vec2<uint16_t> chunkPosition{static_cast<uint16_t>(x), static_cast<uint16_t>(y)};
                QueueChunk(chunkPosition);
            }
        }
    }

    void WorldGenerator::UnloadDistantChunks(const Engine::Vec2<uint16_t>& playerChunkPosition, int viewDistance)
    {
        std::vector<Engine::Vec2<uint16_t>> chunksToUnload{};

        for (const auto& chunkPos: chunks | std::views::keys)
        {
            int deltaX = std::abs(static_cast<int>(chunkPos.x) - static_cast<int>(playerChunkPosition.x));
            int deltaY = std::abs(static_cast<int>(chunkPos.y) - static_cast<int>(playerChunkPosition.y));

            if (deltaX > viewDistance || deltaY > viewDistance)
            {
                chunksToUnload.push_back(chunkPos);
            }
        }

        for (const auto& chunkPos: chunksToUnload)
        {
            chunks.erase(chunkPos);
        }
    }

    const std::map<Engine::Vec2<uint16_t>, std::unique_ptr<Chunk>>& WorldGenerator::GetChunks()
    {
        return chunks;
    }
}
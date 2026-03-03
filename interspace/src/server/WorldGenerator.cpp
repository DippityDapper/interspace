#include "interspace/server/WorldGenerator.hpp"

#include "SDL3/SDL_log.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Tiles.hpp"
#include "interspace/server/World.hpp"

#include <fstream>
#include <ranges>
#include <chrono>

namespace Interspace::Server
{
    WorldGenerator::WorldGenerator(uint32_t _seed)
    {
        seed = _seed;
    }

    uint32_t WorldGenerator::CalculateBytesPerChunk()
    {
        uint32_t tilesDataSize = World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE * (sizeof(uint32_t) * 2);
        return sizeof(bool) + sizeof(uint64_t) + tilesDataSize;
    }

    std::string WorldGenerator::GetRegionPath(uint16_t regionX, uint16_t regionY)
    {
        return "data/server/" + World::worldName + "/regions/" + std::to_string(regionX) + "_" + std::to_string(regionY) + ".region";
    }

    void WorldGenerator::GetRegionCoordinates(Engine::Vec2<uint16_t> chunkPosition, uint16_t& regionX, uint16_t& regionY, uint16_t& regionXIndex, uint16_t& regionYIndex)
    {
        regionX = chunkPosition.x / World::worldData->REGION_SIZE;
        regionY = chunkPosition.y / World::worldData->REGION_SIZE;
        regionXIndex = chunkPosition.x % World::worldData->REGION_SIZE;
        regionYIndex = chunkPosition.y % World::worldData->REGION_SIZE;
    }

    void WorldGenerator::QueueChunk(Engine::Vec2<uint16_t> chunkPosition)
    {
        if (chunks.contains(chunkPosition))
            return;

        chunks.emplace(chunkPosition, std::make_unique<Chunk>());
        Chunk* chunk = chunks[chunkPosition].get();
        chunk->data.position = chunkPosition;

        uint32_t tileSeed = seed ^ (chunk->data.position.x * 73856093) ^ (chunk->data.position.y * 19349663);
        chunk->tileGen.seed(tileSeed);

        if (LoadChunkFromDisk(chunk))
        {
            SDL_Log("[Server] Chunk loaded from disk at (%u, %u).", chunkPosition.x, chunkPosition.y);
        }
        else
        {
            chunkQueue.emplace(chunk);
            SDL_Log("[Server] Chunk queued for generation at (%u, %u).", chunkPosition.x, chunkPosition.y);
        }
    }

    bool WorldGenerator::LoadChunkFromDisk(Chunk* chunk)
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
        }

        regionFile.close();
        return true;
    }

    void WorldGenerator::GenerateChunk(Chunk* chunk)
    {
        for (uint16_t w = 0; w < World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE; w++)
        {
            uint8_t tileX = w % World::worldData->CHUNK_SIZE;
            uint8_t tileY = w / World::worldData->CHUNK_SIZE;
            Engine::Vec2<uint8_t> tilePosition{tileX, tileY};
            chunk->tiles.emplace(tilePosition, Tiles::GetRandomTileBySeed("grass_flower", chunk->tileGen));
        }

        chunk->data.lastModified = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        chunk->data.isModified = false;
        SaveChunk(chunk);
    }

    void WorldGenerator::GenerateNaturalChunks()
    {
        while (!chunkQueue.empty())
        {
            Chunk* chunk = chunkQueue.front();
            chunkQueue.pop();

            GenerateChunk(chunk);
            SDL_Log("[Server] Chunk generated at (%u, %u).", chunk->data.position.x, chunk->data.position.y);
        }
    }

    Chunk* WorldGenerator::GetChunk(Engine::Vec2<uint16_t> chunkPosition)
    {
        if (!chunks.contains(chunkPosition))
            return nullptr;

        Chunk* chunk = chunks[chunkPosition].get();
        if (chunk->tiles.size() < World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE)
            return nullptr;

        return chunk;
    }

    void WorldGenerator::MarkChunkModified(Engine::Vec2<uint16_t> chunkPosition)
    {
        Chunk* chunk = GetChunk(chunkPosition);
        if (!chunk)
            return;

        chunk->data.isModified = true;
        chunk->data.lastModified = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        SDL_Log("[Server] Chunk marked as modified at (%u, %u).", chunkPosition.x, chunkPosition.y);
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

        chunk->data.isModified = false;
        SDL_Log("[Server] Chunk saved at (%u, %u) in region (%u, %u) with timestamp %lu.", chunk->data.position.x, chunk->data.position.y, regionX, regionY, chunk->data.lastModified);
    }

    void WorldGenerator::SaveAllModifiedChunks()
    {
        for (const auto& [position, chunk]: chunks)
        {
            if (chunk->data.isModified)
            {
                SaveChunk(chunk.get());
            }
        }
    }

    uint64_t WorldGenerator::GetChunkTimestamp(Engine::Vec2<uint16_t> chunkPosition)
    {
        Chunk* chunk = GetChunk(chunkPosition);
        if (chunk)
            return chunk->data.lastModified;

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

    void WorldGenerator::AddChunkRequest(uint32_t clientId, Engine::Vec2<uint16_t> chunkPosition, uint64_t chunkTimestamp, uint8_t chunkType)
    {
        ChunkRequest chunkRequest{clientId, chunkPosition.x, chunkPosition.y, chunkTimestamp, chunkType};
        requestQueue.push(chunkRequest);
    }

    void WorldGenerator::HandleChunkRequests()
    {
        int count = requestQueue.size();
        for (int i = 0; i < count; i++)
        {
            ChunkRequest chunkRequest = requestQueue.front();
            Engine::Vec2<uint16_t> chunkPosition{chunkRequest.x, chunkRequest.y};

            if (chunkRequest.chunkType == 1)
            {
            }
            if (chunks.contains(chunkPosition))
            {
                if (!Game::server->PeerExists(chunkRequest.clientId))
                {
                    requestQueue.pop();
                    continue;
                }

                Chunk* chunk = GetChunk(chunkPosition);
                if (!chunk)
                {
                    QueueChunk(chunkPosition);
                    continue;
                }

                if (chunk->data.lastModified <= chunkRequest.timestamp)
                {
                    requestQueue.pop();
                    continue;
                }

                ENetPeer* peer = Game::server->GetPeer(chunkRequest.clientId);
                requestQueue.pop();
                SendChunkToClient(peer, chunk);
            }
            else
            {
                QueueChunk(chunkPosition);
            }
        }
    }

    void WorldGenerator::BroadcastChunks(Player* player)
    {
        int playerChunkX = player->position.x / (static_cast<float>(World::worldData->CHUNK_SIZE) * static_cast<float>(World::worldData->TILE_SIZE));
        int playerChunkY = player->position.y / (static_cast<float>(World::worldData->CHUNK_SIZE) * static_cast<float>(World::worldData->TILE_SIZE));

        int minX = std::max(0, playerChunkX - 3);
        int maxX = std::min(static_cast<int>(World::worldData->worldSizeX), playerChunkX + 3);
        int minY = std::max(0, playerChunkY - 3);
        int maxY = std::min(static_cast<int>(World::worldData->worldSizeY), playerChunkY + 3);

        std::set<Engine::Vec2<uint16_t>> visibleChunks{};

        for (uint16_t x = minX; x < maxX; ++x)
        {
            for (uint16_t y = minY; y < maxY; ++y)
            {
                Engine::Vec2<uint16_t> chunkPos{x, y};
                visibleChunks.insert(chunkPos);

                if (!chunks.contains(chunkPos))
                {
                    QueueChunk(chunkPos);
                }
            }
        }

        std::vector<Engine::Vec2<uint16_t>> chunksToUnload;
        for (const auto& chunkPos: chunks | std::views::keys)
        {
            if (!visibleChunks.contains(chunkPos))
            {
                chunksToUnload.push_back(chunkPos);
            }
        }

        for (const auto& chunkPos: chunksToUnload)
        {
            if (chunks[chunkPos]->data.isModified)
            {
                SaveChunk(chunks[chunkPos].get());
            }
            chunks.erase(chunkPos);
        }
    }

    const std::map<Engine::Vec2<uint16_t>, std::unique_ptr<Chunk>>& WorldGenerator::GetChunks()
    {
        return chunks;
    }
}
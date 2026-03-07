#include "interspace/server/ServerWorldGenerator.hpp"

#include "SDL3/SDL_log.h"
#include "interspace/game/Game.hpp"
#include "interspace/server/ServerWorld.hpp"

#include <fstream>
#include <ranges>

namespace Interspace::Server
{
    ServerWorldGenerator::ServerWorldGenerator(uint32_t _seed)
    {
        seed = _seed;
    }

    uint32_t ServerWorldGenerator::CalculateBytesPerChunk()
    {
        uint32_t tilesDataSize = Game::serverWorld->worldData->CHUNK_SIZE * Game::serverWorld->worldData->CHUNK_SIZE * (sizeof(uint32_t) * 2);
        return sizeof(bool) + sizeof(uint64_t) + tilesDataSize;
    }

    std::string ServerWorldGenerator::GetRegionPath(uint16_t regionX, uint16_t regionY) const
    {
        return "data/server/" + Game::serverWorld->worldName + "/regions/" + std::to_string(regionX) + "_" + std::to_string(regionY) + ".region";
    }

    void ServerWorldGenerator::GetRegionCoordinates(const Engine::Vec2<uint16_t>& chunkPosition, uint16_t& regionX, uint16_t& regionY, uint16_t& regionXIndex, uint16_t& regionYIndex) const
    {
        regionX = chunkPosition.x / Game::serverWorld->worldData->REGION_SIZE;
        regionY = chunkPosition.y / Game::serverWorld->worldData->REGION_SIZE;
        regionXIndex = chunkPosition.x % Game::serverWorld->worldData->REGION_SIZE;
        regionYIndex = chunkPosition.y % Game::serverWorld->worldData->REGION_SIZE;
    }

    void ServerWorldGenerator::QueueChunk(const Engine::Vec2<uint16_t>& chunkPosition)
    {
        if (chunks.contains(chunkPosition))
            return;

        chunks.emplace(chunkPosition, std::make_unique<ServerChunk>());
        ServerChunk* chunk = chunks[chunkPosition].get();
        chunk->position = chunkPosition;

        uint32_t tileSeed = seed ^ (chunk->position.x * 73856093) ^ (chunk->position.y * 19349663);
        chunk->tileGen.seed(tileSeed);

        if (!LoadChunkFromDisk(chunk))
        {
            chunkQueue.emplace(chunk);
        }
    }

    bool ServerWorldGenerator::LoadChunkFromDisk(ServerChunk* chunk)
    {
        uint16_t regionX, regionY, regionXIndex, regionYIndex;
        GetRegionCoordinates(chunk->position, regionX, regionY, regionXIndex, regionYIndex);
        std::string regionPath = GetRegionPath(regionX, regionY);

        if (!std::filesystem::exists(regionPath))
            return false;

        std::ifstream regionFile{regionPath, std::ios::binary};
        if (!regionFile.is_open())
            return false;

        uint32_t bytesPerChunk = CalculateBytesPerChunk();
        uint32_t chunkIndexInRegion = regionYIndex * Game::serverWorld->worldData->REGION_SIZE + regionXIndex;
        uint32_t cursorPos = chunkIndexInRegion * bytesPerChunk;

        regionFile.seekg(0, std::ios::end);
        std::streampos fileSize = regionFile.tellg();
        uint32_t expectedFileSize = Game::serverWorld->worldData->REGION_SIZE * Game::serverWorld->worldData->REGION_SIZE * bytesPerChunk;

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
        chunk->lastModified = timestamp;

        for (int i = 0; i < Game::serverWorld->worldData->CHUNK_SIZE * Game::serverWorld->worldData->CHUNK_SIZE; i++)
        {
            uint32_t tileId = 0;
            regionFile.read(reinterpret_cast<char*>(&tileId), sizeof(uint32_t));
            uint32_t tileVariant = 0;
            regionFile.read(reinterpret_cast<char*>(&tileVariant), sizeof(uint32_t));

            uint8_t tileX = i % Game::serverWorld->worldData->CHUNK_SIZE;
            uint8_t tileY = i / Game::serverWorld->worldData->CHUNK_SIZE;
            Engine::Vec2<uint8_t> tilePosition{tileX, tileY};

            chunk->tiles.emplace(tilePosition, Game::serverWorld->tileRegistry->GetTileOfType(tileId, tileVariant));
        }

        regionFile.close();
        return true;
    }

    void ServerWorldGenerator::Generate()
    {
        while (!chunkQueue.empty())
        {
            ServerChunk* chunk = chunkQueue.front();
            chunkQueue.pop();

            GenerateChunk(chunk);
        }
    }

    ServerChunk* ServerWorldGenerator::GetChunk(const Engine::Vec2<uint16_t>& chunkPosition)
    {
        if (!chunks.contains(chunkPosition))
            return nullptr;

        ServerChunk* chunk = chunks[chunkPosition].get();
        if (chunk->tiles.size() < Game::serverWorld->worldData->CHUNK_SIZE * Game::serverWorld->worldData->CHUNK_SIZE)
            return nullptr;

        return chunk;
    }

    void ServerWorldGenerator::MarkChunkModified(const Engine::Vec2<uint16_t>& chunkPosition)
    {
        ServerChunk* chunk = GetChunk(chunkPosition);
        if (!chunk)
            return;

        chunk->isModified = true;
        chunk->lastModified = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void ServerWorldGenerator::SaveChunk(ServerChunk* chunk)
    {
        if (!chunk)
            return;

        if (chunk->tiles.size() < Game::serverWorld->worldData->CHUNK_SIZE * Game::serverWorld->worldData->CHUNK_SIZE)
            return;

        uint16_t regionX, regionY, regionXIndex, regionYIndex;
        GetRegionCoordinates(chunk->position, regionX, regionY, regionXIndex, regionYIndex);
        std::string regionPath = GetRegionPath(regionX, regionY);

        uint32_t bytesPerChunk = CalculateBytesPerChunk();
        uint32_t chunkIndexInRegion = regionYIndex * Game::serverWorld->worldData->REGION_SIZE + regionXIndex;
        uint32_t cursorPos = chunkIndexInRegion * bytesPerChunk;

        if (std::filesystem::exists(regionPath))
        {
            std::fstream regionFile{regionPath, std::ios::binary | std::ios::in | std::ios::out};
            regionFile.seekp(cursorPos, std::ios::beg);

            bool chunkGenerated = true;
            regionFile.write(reinterpret_cast<const char*>(&chunkGenerated), sizeof(bool));

            uint64_t timestamp = chunk->lastModified;
            regionFile.write(reinterpret_cast<const char*>(&timestamp), sizeof(uint64_t));

            for (int w = 0; w < Game::serverWorld->worldData->CHUNK_SIZE * Game::serverWorld->worldData->CHUNK_SIZE; w++)
            {
                uint8_t tileX = w % Game::serverWorld->worldData->CHUNK_SIZE;
                uint8_t tileY = w / Game::serverWorld->worldData->CHUNK_SIZE;
                Engine::Vec2<uint8_t> tilePosition{tileX, tileY};

                if (chunk->tiles.contains(tilePosition))
                {
                    Tile* tile = chunk->tiles[tilePosition];
                    uint32_t tileId = tile->tileId;
                    uint32_t tileVariant = tile->variant;
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

            uint16_t chunkCount = Game::serverWorld->worldData->REGION_SIZE * Game::serverWorld->worldData->REGION_SIZE;
            for (uint16_t i = 0; i < chunkCount; i++)
            {
                uint16_t chunkXInRegion = i % Game::serverWorld->worldData->REGION_SIZE;
                uint16_t chunkYInRegion = i / Game::serverWorld->worldData->REGION_SIZE;
                Engine::Vec2<uint16_t> chunkPos{
                        static_cast<uint16_t>(regionX * Game::serverWorld->worldData->REGION_SIZE + chunkXInRegion),
                        static_cast<uint16_t>(regionY * Game::serverWorld->worldData->REGION_SIZE + chunkYInRegion)};

                ServerChunk* currentChunk = nullptr;
                if (chunks.contains(chunkPos))
                    currentChunk = chunks[chunkPos].get();

                bool chunkGenerated = currentChunk != nullptr && currentChunk->tiles.size() >= Game::serverWorld->worldData->CHUNK_SIZE * Game::serverWorld->worldData->CHUNK_SIZE;
                regionFile.write(reinterpret_cast<const char*>(&chunkGenerated), sizeof(bool));

                uint64_t timestamp = 0;
                if (currentChunk)
                    timestamp = currentChunk->lastModified;

                regionFile.write(reinterpret_cast<const char*>(&timestamp), sizeof(uint64_t));

                for (int w = 0; w < Game::serverWorld->worldData->CHUNK_SIZE * Game::serverWorld->worldData->CHUNK_SIZE; w++)
                {
                    uint8_t tileX = w % Game::serverWorld->worldData->CHUNK_SIZE;
                    uint8_t tileY = w / Game::serverWorld->worldData->CHUNK_SIZE;
                    Engine::Vec2<uint8_t> tilePosition{tileX, tileY};

                    if (currentChunk && currentChunk->tiles.contains(tilePosition))
                    {
                        Tile* tile = currentChunk->tiles[tilePosition];
                        uint32_t tileId = tile->tileId;
                        uint32_t tileVariant = tile->variant;
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

        chunk->isModified = false;
    }

    void ServerWorldGenerator::SaveAllModifiedChunks()
    {
        for (const auto& [position, chunk]: chunks)
        {
            if (chunk->isModified)
            {
                SaveChunk(chunk.get());
            }
        }
    }

    uint64_t ServerWorldGenerator::GetChunkTimestamp(const Engine::Vec2<uint16_t>& chunkPosition)
    {
        ServerChunk* chunk = GetChunk(chunkPosition);
        if (chunk)
            return chunk->lastModified;

        uint16_t regionX, regionY, regionXIndex, regionYIndex;
        GetRegionCoordinates(chunkPosition, regionX, regionY, regionXIndex, regionYIndex);
        std::string regionPath = GetRegionPath(regionX, regionY);

        if (!std::filesystem::exists(regionPath))
            return 0;

        std::ifstream regionFile{regionPath, std::ios::binary};
        if (!regionFile.is_open())
            return 0;

        uint32_t bytesPerChunk = CalculateBytesPerChunk();
        uint32_t chunkIndexInRegion = regionYIndex * Game::serverWorld->worldData->REGION_SIZE + regionXIndex;
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

    void ServerWorldGenerator::AddChunkRequest(client_id_t clientId, const ChunkRequestPacket& requestPacket)
    {
        ChunkRequest request{
                .clientId = clientId,
                .x = requestPacket.x,
                .y = requestPacket.y,
                .timestamp = requestPacket.timestamp,
                .type = static_cast<ChunkType>(requestPacket.type)};

        requestQueue.push(request);
    }

    void ServerWorldGenerator::HandleChunkRequests()
    {
        int count = requestQueue.size();
        for (int i = 0; i < count; i++)
        {
            ChunkRequest chunkRequest = requestQueue.front();
            Engine::Vec2<uint16_t> chunkPosition{chunkRequest.x, chunkRequest.y};

            if (chunkRequest.type == 1)
            {
            }
            if (chunks.contains(chunkPosition))
            {
                if (!Game::server->PeerExists(chunkRequest.clientId))
                {
                    requestQueue.pop();
                    continue;
                }

                ServerChunk* chunk = GetChunk(chunkPosition);
                if (!chunk)
                {
                    QueueChunk(chunkPosition);
                    continue;
                }

                if (chunk->lastModified <= chunkRequest.timestamp)
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

    void ServerWorldGenerator::BroadcastChunks(Player* player)
    {
        int playerChunkX = player->position.x / (static_cast<float>(Game::serverWorld->worldData->CHUNK_SIZE) * static_cast<float>(Game::serverWorld->worldData->TILE_SIZE));
        int playerChunkY = player->position.y / (static_cast<float>(Game::serverWorld->worldData->CHUNK_SIZE) * static_cast<float>(Game::serverWorld->worldData->TILE_SIZE));

        int minX = std::max(0, playerChunkX - 3);
        int maxX = std::min(static_cast<int>(Game::serverWorld->worldData->worldSizeX), playerChunkX + 3);
        int minY = std::max(0, playerChunkY - 3);
        int maxY = std::min(static_cast<int>(Game::serverWorld->worldData->worldSizeY), playerChunkY + 3);

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
            if (chunks[chunkPos]->isModified)
            {
                SaveChunk(chunks[chunkPos].get());
            }
            chunks.erase(chunkPos);
        }
    }

    const std::map<Engine::Vec2<uint16_t>, std::unique_ptr<ServerChunk>>& ServerWorldGenerator::GetChunks()
    {
        return chunks;
    }
}
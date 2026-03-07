#include "interspace/client/ClientWorldGenerator.hpp"

#include "SDL3/SDL_log.h"
#include "interspace/client/ClientWorld.hpp"
#include "interspace/game/Game.hpp"

#include <fstream>
#include <ranges>

namespace Interspace::Client
{
    ClientWorldGenerator::ClientWorldGenerator(uint32_t _seed)
    {
        seed = _seed;
    }
    uint32_t ClientWorldGenerator::CalculateBytesPerChunk() const
    {
        uint32_t tilesDataSize = Game::clientWorld->worldData->CHUNK_SIZE * Game::clientWorld->worldData->CHUNK_SIZE * (sizeof(uint32_t) * 2);
        return sizeof(bool) + sizeof(uint64_t) + tilesDataSize;
    }

    std::string ClientWorldGenerator::GetRegionPath(uint16_t regionX, uint16_t regionY) const
    {
        return "data/client/" + Game::clientWorld->worldName + "/regions/" + std::to_string(regionX) + "_" + std::to_string(regionY) + ".region";
    }

    void ClientWorldGenerator::GetRegionCoordinates(const Engine::Vec2<uint16_t>& chunkPosition, uint16_t& regionX, uint16_t& regionY, uint16_t& regionXIndex, uint16_t& regionYIndex) const
    {
        regionX = chunkPosition.x / Game::clientWorld->worldData->REGION_SIZE;
        regionY = chunkPosition.y / Game::clientWorld->worldData->REGION_SIZE;
        regionXIndex = chunkPosition.x % Game::clientWorld->worldData->REGION_SIZE;
        regionYIndex = chunkPosition.y % Game::clientWorld->worldData->REGION_SIZE;
    }

    void ClientWorldGenerator::Tick()
    {
        uint16_t playerChunkX = Engine::Camera::main->position.x / (static_cast<float>(Game::clientWorld->worldData->CHUNK_SIZE) * static_cast<float>(Game::clientWorld->worldData->TILE_SIZE));
        uint16_t playerChunkY = Engine::Camera::main->position.y / (static_cast<float>(Game::clientWorld->worldData->CHUNK_SIZE) * static_cast<float>(Game::clientWorld->worldData->TILE_SIZE));

        TryLoadChunks({playerChunkX, playerChunkY}, 3);
        UnloadDistantChunks({playerChunkX, playerChunkY}, 3);

        std::set<Engine::Vec2<uint16_t>> visibleChunks{};
    }

    void ClientWorldGenerator::QueueChunk(const Engine::Vec2<uint16_t>& chunkPosition)
    {
        if (chunks.contains(chunkPosition))
            return;

        chunks.emplace(chunkPosition, std::make_unique<ClientChunk>(chunkPosition));
        ClientChunk* chunk = chunks[chunkPosition].get();
        chunk->position = chunkPosition;
        chunk->lastModified = 0;

        if (LoadChunkFromDisk(chunk))
        {
            RequestChunkNatural(chunk);
        }
        else
        {
            RequestChunkNatural(chunk);
        }
    }

    bool ClientWorldGenerator::LoadChunkFromDisk(ClientChunk* chunk)
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
        uint32_t chunkIndexInRegion = regionYIndex * Game::clientWorld->worldData->REGION_SIZE + regionXIndex;
        uint32_t cursorPos = chunkIndexInRegion * bytesPerChunk;

        regionFile.seekg(0, std::ios::end);
        std::streampos fileSize = regionFile.tellg();
        uint32_t expectedFileSize = Game::clientWorld->worldData->REGION_SIZE * Game::clientWorld->worldData->REGION_SIZE * bytesPerChunk;

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

        chunk->BeginTileUpdate();
        for (int i = 0; i < Game::clientWorld->worldData->CHUNK_SIZE * Game::clientWorld->worldData->CHUNK_SIZE; i++)
        {
            uint32_t tileId = 0;
            regionFile.read(reinterpret_cast<char*>(&tileId), sizeof(uint32_t));
            uint32_t tileVariant = 0;
            regionFile.read(reinterpret_cast<char*>(&tileVariant), sizeof(uint32_t));

            uint8_t tileX = i % Game::clientWorld->worldData->CHUNK_SIZE;
            uint8_t tileY = i / Game::clientWorld->worldData->CHUNK_SIZE;
            Engine::Vec2<uint8_t> tilePosition{tileX, tileY};

            chunk->tiles.emplace(tilePosition, Game::clientWorld->tileRegistry->GetTileOfType(tileId, tileVariant));
            chunk->UpdateTile(tilePosition, chunk->tiles[tilePosition]);
        }
        chunk->EndTileUpdate();

        regionFile.close();
        return true;
    }

    ClientChunk* ClientWorldGenerator::GetChunk(const Engine::Vec2<uint16_t>& chunkPosition)
    {
        if (!chunks.contains(chunkPosition))
            return nullptr;

        ClientChunk* chunk = chunks[chunkPosition].get();

        return chunk;
    }

    void ClientWorldGenerator::SaveChunk(const Engine::Vec2<uint16_t>& chunkPosition)
    {
        if (!chunks.contains(chunkPosition))
            return;

        ClientChunk* chunk = chunks[chunkPosition].get();
        SaveChunk(chunk);
    }

    void ClientWorldGenerator::SaveChunk(ClientChunk* chunk)
    {
        if (!chunk)
            return;

        if (chunk->tiles.size() < Game::clientWorld->worldData->CHUNK_SIZE * Game::clientWorld->worldData->CHUNK_SIZE)
            return;

        uint16_t regionX, regionY, regionXIndex, regionYIndex;
        GetRegionCoordinates(chunk->position, regionX, regionY, regionXIndex, regionYIndex);
        std::string regionPath = GetRegionPath(regionX, regionY);

        uint32_t bytesPerChunk = CalculateBytesPerChunk();
        uint32_t chunkIndexInRegion = regionYIndex * Game::clientWorld->worldData->REGION_SIZE + regionXIndex;
        uint32_t cursorPos = chunkIndexInRegion * bytesPerChunk;

        if (std::filesystem::exists(regionPath))
        {
            std::fstream regionFile{regionPath, std::ios::binary | std::ios::in | std::ios::out};
            regionFile.seekp(cursorPos, std::ios::beg);

            bool chunkGenerated = true;
            regionFile.write(reinterpret_cast<const char*>(&chunkGenerated), sizeof(bool));

            uint64_t timestamp = chunk->lastModified;
            regionFile.write(reinterpret_cast<const char*>(&timestamp), sizeof(uint64_t));

            for (int w = 0; w < Game::clientWorld->worldData->CHUNK_SIZE * Game::clientWorld->worldData->CHUNK_SIZE; w++)
            {
                uint8_t tileX = w % Game::clientWorld->worldData->CHUNK_SIZE;
                uint8_t tileY = w / Game::clientWorld->worldData->CHUNK_SIZE;
                Engine::Vec2<uint8_t> tilePosition{tileX, tileY};

                if (chunk->tiles.contains(tilePosition))
                {
                    ClientTile* tile = chunk->tiles[tilePosition];
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

            uint16_t chunkCount = Game::clientWorld->worldData->REGION_SIZE * Game::clientWorld->worldData->REGION_SIZE;
            for (uint16_t i = 0; i < chunkCount; i++)
            {
                uint16_t chunkXInRegion = i % Game::clientWorld->worldData->REGION_SIZE;
                uint16_t chunkYInRegion = i / Game::clientWorld->worldData->REGION_SIZE;
                Engine::Vec2<uint16_t> chunkPos{
                        static_cast<uint16_t>(regionX * Game::clientWorld->worldData->REGION_SIZE + chunkXInRegion),
                        static_cast<uint16_t>(regionY * Game::clientWorld->worldData->REGION_SIZE + chunkYInRegion)};

                ClientChunk* currentChunk = nullptr;
                if (chunks.contains(chunkPos))
                    currentChunk = chunks[chunkPos].get();

                bool chunkGenerated = currentChunk != nullptr && currentChunk->tiles.size() >= Game::clientWorld->worldData->CHUNK_SIZE * Game::clientWorld->worldData->CHUNK_SIZE;
                regionFile.write(reinterpret_cast<const char*>(&chunkGenerated), sizeof(bool));

                uint64_t timestamp = 0;
                if (currentChunk)
                    timestamp = currentChunk->lastModified;

                regionFile.write(reinterpret_cast<const char*>(&timestamp), sizeof(uint64_t));

                for (int w = 0; w < Game::clientWorld->worldData->CHUNK_SIZE * Game::clientWorld->worldData->CHUNK_SIZE; w++)
                {
                    uint8_t tileX = w % Game::clientWorld->worldData->CHUNK_SIZE;
                    uint8_t tileY = w / Game::clientWorld->worldData->CHUNK_SIZE;
                    Engine::Vec2<uint8_t> tilePosition{tileX, tileY};

                    if (currentChunk && currentChunk->tiles.contains(tilePosition))
                    {
                        ClientTile* tile = currentChunk->tiles[tilePosition];
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
    }

    std::map<Engine::Vec2<uint16_t>, uint64_t> ClientWorldGenerator::GetChunkTimestamps(const std::set<Engine::Vec2<uint16_t>>& chunkPositions)
    {
        std::map<Engine::Vec2<uint16_t>, uint64_t> timestamps;

        for (const auto& chunkPos: chunkPositions)
        {
            uint64_t timestamp = GetChunkTimestamp(chunkPos);
            timestamps[chunkPos] = timestamp;
        }

        return timestamps;
    }

    uint64_t ClientWorldGenerator::GetChunkTimestamp(const Engine::Vec2<uint16_t>& chunkPosition)
    {
        if (chunks.contains(chunkPosition))
        {
            ClientChunk* chunk = chunks[chunkPosition].get();
            if (chunk->tiles.size() >= Game::clientWorld->worldData->CHUNK_SIZE * Game::clientWorld->worldData->CHUNK_SIZE)
            {
                return chunk->lastModified;
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
        uint32_t chunkIndexInRegion = regionYIndex * Game::clientWorld->worldData->REGION_SIZE + regionXIndex;
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

    void ClientWorldGenerator::ReceiveChunkFromServer(const ChunkPacket& chunkPacket)
    {
        Engine::Vec2<uint16_t> chunkPosition{chunkPacket.x, chunkPacket.y};
        if (!chunks.contains(chunkPosition))
        {
            chunks.emplace(chunkPosition, std::make_unique<ClientChunk>(chunkPosition));
        }

        ClientChunk* chunk = chunks[chunkPosition].get();
        chunk->position = chunkPosition;
        chunk->lastModified = chunkPacket.timestamp;
        chunk->tiles.clear();

        if (chunkPacket.tiles.size() != Game::clientWorld->worldData->CHUNK_SIZE * Game::clientWorld->worldData->CHUNK_SIZE)
        {
            SDL_Log("[Client] ERROR: Received chunk data size mismatch at (%u, %u). Expected %u, got %zu.",
                    chunkPosition.x,
                    chunkPosition.y,
                    Game::clientWorld->worldData->CHUNK_SIZE * Game::clientWorld->worldData->CHUNK_SIZE,
                    chunkPacket.tiles.size());
            return;
        }

        chunk->BeginTileUpdate();
        for (int i = 0; i < chunkPacket.tiles.size(); i++)
        {
            uint8_t tileX = i % Game::clientWorld->worldData->CHUNK_SIZE;
            uint8_t tileY = i / Game::clientWorld->worldData->CHUNK_SIZE;
            Engine::Vec2<uint8_t> tilePosition{tileX, tileY};

            uint32_t tileId = chunkPacket.tiles[i].id;
            uint32_t tileVariant = chunkPacket.tiles[i].variant;

            chunk->tiles.emplace(tilePosition, Game::clientWorld->tileRegistry->GetTileOfType(tileId, tileVariant));
            chunk->UpdateTile(tilePosition, chunk->tiles[tilePosition]);
        }
        chunk->EndTileUpdate();

        SaveChunk(chunk);
    }

    void ClientWorldGenerator::TryLoadChunks(const Engine::Vec2<uint16_t>& playerChunkPosition, int viewDistance)
    {
        for (int y = playerChunkPosition.y - viewDistance; y <= playerChunkPosition.y + viewDistance; ++y)
        {
            for (int x = playerChunkPosition.x - viewDistance; x <= playerChunkPosition.x + viewDistance; ++x)
            {
                if (x < 0 || x > Game::clientWorld->worldData->worldSizeX || y < 0 || y > Game::clientWorld->worldData->worldSizeY)
                    continue;

                Engine::Vec2<uint16_t> chunkPosition{static_cast<uint16_t>(x), static_cast<uint16_t>(y)};
                QueueChunk(chunkPosition);
            }
        }
    }

    void ClientWorldGenerator::UnloadDistantChunks(const Engine::Vec2<uint16_t>& playerChunkPosition, int viewDistance)
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

    const std::map<Engine::Vec2<uint16_t>, std::unique_ptr<ClientChunk>>& ClientWorldGenerator::GetChunks()
    {
        return chunks;
    }
}
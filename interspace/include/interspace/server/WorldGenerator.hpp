#pragma once

#include "enet/enet.h"
#include "interspace/server/Chunk.hpp"
#include "interspace/world/WorldData.hpp"

#include <memory>
#include <queue>
#include <cstdint>
#include <chrono>

namespace Interspace::Server
{
    class Player;

    struct ChunkRequest
    {
        uint32_t clientId = 0;
        uint16_t x = 0;
        uint16_t y = 0;
        uint64_t timestamp = 0;
        uint8_t chunkType = 0;
    };

    class WorldGenerator
    {
      private:
        uint32_t seed = 0;

        std::map<Engine::Vec2<uint16_t>, std::unique_ptr<Chunk>> chunks{};
        std::queue<Chunk*> chunkQueue{};
        std::queue<ChunkRequest> requestQueue{};

        uint32_t CalculateBytesPerChunk();
        std::string GetRegionPath(uint16_t regionX, uint16_t regionY);
        void GetRegionCoordinates(Engine::Vec2<uint16_t> chunkPosition, uint16_t& regionX, uint16_t& regionY, uint16_t& regionXIndex, uint16_t& regionYIndex);

        bool LoadChunkFromDisk(Chunk* chunk);
        void GenerateChunk(Chunk* chunk);

      public:
        WorldGenerator(uint32_t _seed);

        void GenerateNaturalChunks();
        void QueueChunk(Engine::Vec2<uint16_t> chunkPosition);

        Chunk* GetChunk(Engine::Vec2<uint16_t> chunkPosition);
        const std::map<Engine::Vec2<uint16_t>, std::unique_ptr<Chunk>>& GetChunks();

        void MarkChunkModified(Engine::Vec2<uint16_t> chunkPosition);
        void SaveChunk(Chunk* chunk);
        void SaveAllModifiedChunks();

        void BroadcastChunks(Player* player);
        void SendChunkToClient(ENetPeer* client, Chunk* chunk);

        uint64_t GetChunkTimestamp(Engine::Vec2<uint16_t> chunkPosition);
        void LoadChunkTimestamp(Chunk* chunk);
        void SaveChunkTimestamp(Chunk* chunk);
        void AddChunkRequest(uint32_t clientId, Engine::Vec2<uint16_t> chunkPosition, uint64_t chunkTimestamp, uint8_t chunkType);
        void HandleChunkRequests();
    };
}
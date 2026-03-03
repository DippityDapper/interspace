#pragma once

#include "interspace/client/Chunk.hpp"
#include "igneous/engine/Vec2.hpp"

#include <cstdint>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>

namespace Interspace::Client
{
    class WorldGenerator
    {
      private:
        std::map<Engine::Vec2<uint16_t>, std::unique_ptr<Chunk>> chunks{};

        uint32_t CalculateBytesPerChunk() const;
        std::string GetRegionPath(uint16_t regionX, uint16_t regionY) const;
        void GetRegionCoordinates(const Engine::Vec2<uint16_t>& chunkPosition, uint16_t& regionX, uint16_t& regionY, uint16_t& regionXIndex, uint16_t& regionYIndex) const;

      private:
        void RequestChunkNatural(Chunk* chunk);

      public:
        void Tick();
        void QueueChunk(const Engine::Vec2<uint16_t>& chunkPosition);
        bool LoadChunkFromCache(Chunk* chunk);

        Chunk* GetChunk(const Engine::Vec2<uint16_t>& chunkPosition);
        const std::map<Engine::Vec2<uint16_t>, std::unique_ptr<Chunk>>& GetChunks();

        void SaveChunk(const Engine::Vec2<uint16_t>& chunkPosition);
        void SaveChunk(Chunk* chunk);
        std::map<Engine::Vec2<uint16_t>, uint64_t> GetChunkTimestamps(const std::set<Engine::Vec2<uint16_t>>& chunkPositions);
        void ReceiveChunkFromServer(const Engine::Vec2<uint16_t>& chunkPosition, uint64_t timestamp, const std::vector<std::pair<uint32_t, uint32_t>>& tileData);

        void TryLoadChunks(const Engine::Vec2<uint16_t>& playerChunkPosition, int viewDistance);
        void UnloadDistantChunks(const Engine::Vec2<uint16_t>& playerChunkPosition, int viewDistance);

        uint64_t GetChunkTimestamp(const Engine::Vec2<uint16_t>& chunkPosition);
    };
}
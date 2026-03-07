#pragma once

#include "interspace/client/ClientChunk.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/world/WorldGenerator.hpp"

#include <set>
#include <vector>

namespace Interspace::Client
{
    class ClientWorldGenerator : public WorldGenerator<ClientChunk>
    {
      private:
        uint32_t CalculateBytesPerChunk() const;
        void RequestChunkNatural(ClientChunk* chunk);
        uint64_t GetChunkTimestamp(const Engine::Vec2<uint16_t>& chunkPosition);
        std::string GetRegionPath(uint16_t regionX, uint16_t regionY) const;
        void GetRegionCoordinates(const Engine::Vec2<uint16_t>& chunkPosition, uint16_t& regionX, uint16_t& regionY, uint16_t& regionXIndex, uint16_t& regionYIndex) const;

      public:
        ClientWorldGenerator(uint32_t _seed);
        void GenerateChunk(ClientChunk* chunk) override = 0;
        bool LoadChunkFromDisk(ClientChunk* chunk) override;
        void SaveChunk(ClientChunk* chunk) override;

        void Tick();
        void QueueChunk(const Engine::Vec2<uint16_t>& chunkPosition);
        void SaveChunk(const Engine::Vec2<uint16_t>& chunkPosition);
        ClientChunk* GetChunk(const Engine::Vec2<uint16_t>& chunkPosition);
        const std::map<Engine::Vec2<uint16_t>, std::unique_ptr<ClientChunk>>& GetChunks();
        std::map<Engine::Vec2<uint16_t>, uint64_t> GetChunkTimestamps(const std::set<Engine::Vec2<uint16_t>>& chunkPositions);
        void ReceiveChunkFromServer(const ChunkPacket& chunkPacket);
        void TryLoadChunks(const Engine::Vec2<uint16_t>& playerChunkPosition, int viewDistance);
        void UnloadDistantChunks(const Engine::Vec2<uint16_t>& playerChunkPosition, int viewDistance);
    };
}
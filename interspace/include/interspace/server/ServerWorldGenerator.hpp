#pragma once

#include "interspace/server/ServerChunk.hpp"
#include "enet/enet.h"
#include "interspace/game/Typedefs.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/world/ChunkTypes.hpp"
#include "interspace/world/WorldGenerator.hpp"

#include <queue>

namespace Interspace::Server
{
    class Player;

    struct ChunkRequest
    {
        client_id_t clientId = 0;
        uint16_t x = 0;
        uint16_t y = 0;
        uint64_t timestamp = 0;
        ChunkType type = Natural;
    };

    class ServerWorldGenerator : public WorldGenerator<ServerChunk>
    {
      private:
        std::queue<ServerChunk*> chunkQueue{};
        std::queue<ChunkRequest> requestQueue{};

      private:
        uint32_t CalculateBytesPerChunk();
        uint64_t GetChunkTimestamp(const Engine::Vec2<uint16_t>& chunkPosition);
        std::string GetRegionPath(uint16_t regionX, uint16_t regionY) const;
        void GetRegionCoordinates(const Engine::Vec2<uint16_t>& chunkPosition, uint16_t& regionX, uint16_t& regionY, uint16_t& regionXIndex, uint16_t& regionYIndex) const;

      public:
        ServerWorldGenerator(uint32_t _seed);

        void GenerateChunk(ServerChunk* chunk) override = 0;
        bool LoadChunkFromDisk(ServerChunk* chunk) override;
        void SaveChunk(ServerChunk* chunk) override;

        void Generate();
        void QueueChunk(const Engine::Vec2<uint16_t>& chunkPosition);
        ServerChunk* GetChunk(const Engine::Vec2<uint16_t>& chunkPosition);
        const std::map<Engine::Vec2<uint16_t>, std::unique_ptr<ServerChunk>>& GetChunks();
        void MarkChunkModified(const Engine::Vec2<uint16_t>& chunkPosition);
        void SaveAllModifiedChunks();
        void BroadcastChunks(Player* player);
        void SendChunkToClient(ENetPeer* peer, ServerChunk* chunk);
        void AddChunkRequest(client_id_t clientId, const ChunkRequestPacket& requestPacket);
        void HandleChunkRequests();
    };
}
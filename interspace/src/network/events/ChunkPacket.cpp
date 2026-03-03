#include "SDL3/SDL_log.h"
#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/World.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/server/World.hpp"

#include <ranges>
#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Server
    {
        void WorldGenerator::SendChunkToClient(ENetPeer* client, Chunk* chunk)
        {
            if (!chunk)
                return;
            if (chunk->tiles.size() < World::worldData->CHUNK_SIZE * World::worldData->CHUNK_SIZE)
                return;

            std::vector<uint8_t> chunkData{CHUNK_PACKET};
            Engine::Serializer serializer(chunkData);

            serializer << chunk->data.position.x << chunk->data.position.y << chunk->data.lastModified;

            for (const auto& tile: chunk->tiles | std::views::values)
            {
                serializer << tile->data.tileId << tile->data.variant;
            }

            Game::server->netInterface->SendToClient(client, chunkData, ENET_PACKET_FLAG_RELIABLE);
            SDL_Log("[Server] Sent chunk (%u, %u) to player with timestamp %lu.", chunk->data.position.x, chunk->data.position.y, chunk->data.lastModified);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Client
    {
        void World::OnChunkPacketReceived(const std::vector<uint8_t>& data)
        {
            uint16_t chunkX = 0;
            uint16_t chunkY = 0;
            uint64_t timestamp = 0;

            Engine::Deserializer deserializer(data);
            deserializer >> chunkX >> chunkY >> timestamp;
            Engine::Vec2<uint16_t> chunkPosition{chunkX, chunkY};

            if (chunks.contains(chunkPosition))
            {
                Chunk* currentChunk = chunks[chunkPosition].get();
                if (currentChunk->data.lastModified >= timestamp)
                    return;
            }

            std::vector<std::pair<uint32_t, uint32_t>> tileData{};
            for (int i = 0; i < worldData->CHUNK_SIZE * worldData->CHUNK_SIZE; ++i)
            {
                tileData.emplace_back(0, 0);
                deserializer >> tileData[i].first >> tileData[i].second;
            }

            worldGenerator->ReceiveChunkFromServer(chunkPosition, timestamp, tileData);
        }
    }
}
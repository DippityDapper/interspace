#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/ClientWorld.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/server/ServerWorld.hpp"

#include <ranges>
#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Server
    {
        void ServerWorldGenerator::SendChunkToClient(ENetPeer* peer, ServerChunk* chunk)
        {
            if (!chunk)
                return;
            if (chunk->tiles.size() < Game::serverWorld->worldData->CHUNK_SIZE * Game::serverWorld->worldData->CHUNK_SIZE)
                return;

            std::vector<uint8_t> chunkData{CHUNK_PACKET};
            Engine::Serializer serializer(chunkData);

            ChunkPacket chunkPacket{
                    .x = chunk->position.x,
                    .y = chunk->position.y,
                    .timestamp = chunk->lastModified};

            for (const auto& tile: chunk->tiles | std::views::values)
                chunkPacket.tiles.emplace_back(tile->tileId, tile->variant);

            serializer << chunkPacket;

            Game::server->netInterface->SendToClient(peer, chunkData, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Client
    {
        void ClientWorld::OnChunkPacketReceived(const std::vector<uint8_t>& data)
        {
            Engine::Deserializer deserializer(data);

            ChunkPacket chunkPacket{};
            deserializer >> chunkPacket;

            Engine::Vec2<uint16_t> chunkPosition{chunkPacket.x, chunkPacket.y};

            if (worldGenerator->chunks.contains(chunkPosition))
            {
                ClientChunk* currentChunk = worldGenerator->GetChunk(chunkPosition);
                if (currentChunk->lastModified >= chunkPacket.timestamp)
                    return;
            }

            worldGenerator->ReceiveChunkFromServer(chunkPacket);
        }
    }
}
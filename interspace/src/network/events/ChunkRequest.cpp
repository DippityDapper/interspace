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
    namespace Client
    {
        void WorldGenerator::RequestChunkNatural(Chunk* chunk)
        {
            std::vector<uint8_t> data{CHUNK_REQUEST};
            Engine::Serializer serializer{data};
            uint8_t chunkType = 1;
            serializer << Game::client->clientId << chunk->data.position.x << chunk->data.position.y << chunk->data.lastModified << chunkType;
            Game::client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Server
    {
        void World::OnChunkRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            Engine::Deserializer deserializer{data};
            uint32_t clientId = 0;
            uint16_t chunkPositionX = 0;
            uint16_t chunkPositionY = 0;
            uint64_t chunkTimestamp = 0;
            uint8_t chunkType = 0;
            deserializer >> clientId >> chunkPositionX >> chunkPositionY >> chunkTimestamp >> chunkType;

            if (clientId <= 0)
                return;
            if (chunkPositionX > worldData->worldSizeX || chunkPositionY > worldData->worldSizeY)
                return;

            Engine::Vec2<uint16_t> chunkPosition{chunkPositionX, chunkPositionY};
            worldGenerator->AddChunkRequest(clientId, chunkPosition, chunkTimestamp, chunkType);
        }
    }
}
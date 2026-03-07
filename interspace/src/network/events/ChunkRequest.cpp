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
    namespace Client
    {
        void ClientWorldGenerator::RequestChunkNatural(ClientChunk* chunk)
        {
            std::vector<uint8_t> data{CHUNK_REQUEST};
            Engine::Serializer serializer{data};

            ChunkRequestPacket chunkRequestPacket{
                    .x = chunk->position.x,
                    .y = chunk->position.y,
                    .timestamp = chunk->lastModified,
                    .type = 1};

            serializer << chunkRequestPacket;
            Game::client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Server
    {
        void ServerWorld::OnChunkRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            Engine::Deserializer deserializer{data};
            ChunkRequestPacket chunkRequestPacket{};
            deserializer >> chunkRequestPacket;

            if (chunkRequestPacket.x > worldData->worldSizeX || chunkRequestPacket.y > worldData->worldSizeY)
                return;
 if (Game::server->GetClientId(from) == 0)
                return;

            client_id_t clientId = Game::server->GetClientId(from);
            worldGenerator->AddChunkRequest(clientId, chunkRequestPacket);
        }
    }
}
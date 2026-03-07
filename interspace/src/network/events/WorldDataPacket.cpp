#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/ClientOverworldGenerator.hpp"
#include "interspace/client/ClientWorld.hpp"
#include "interspace/server/ServerWorld.hpp"

#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Server
    {
        void ServerWorld::SendWorldData(ENetPeer* to)
        {
            std::vector<uint8_t> data{WORLD_DATA_PACKET};
            Engine::Serializer serializer(data);

            WorldPacket worldPacket{
                    .x = worldData->worldSizeX,
                    .y = worldData->worldSizeY,
                    .name = worldName,
                    .seed = worldGenerator->seed};

            serializer << worldPacket;
            server->netInterface->SendToClient(to, data, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Client
    {
        void ClientWorld::OnWorldDataReceived(const std::vector<uint8_t>& data)
        {
            Engine::Deserializer deserializer(data);
            WorldPacket worldPacket{};
            deserializer >> worldPacket;

            worldData->worldSizeX = worldPacket.x;
            worldData->worldSizeY = worldPacket.y;
            name = worldPacket.name;
            worldGenerator = std::make_unique<ClientOverworldGenerator>(worldPacket.seed);

            std::mt19937 gen(std::random_device{}());

            float posX = 0;
            std::uniform_int_distribution<uint32_t> posXDist(0, worldData->worldSizeX * worldData->CHUNK_SIZE * worldData->TILE_SIZE);
            posX = (float) posXDist(gen);

            float posY = 0;
            std::uniform_int_distribution<uint32_t> posYDist(0, worldData->worldSizeY * worldData->CHUNK_SIZE * worldData->TILE_SIZE);
            posY = (float) posYDist(gen);

            camera = std::make_unique<Camera>(posX, posY, 1.0f);
            camera->SetCurrent();

            camera->minZoom = 0.05;
            camera->limitBounds = true;
            camera->limitLeft = 0.0f;
            camera->limitRight = worldData->worldSizeX * worldData->CHUNK_SIZE * worldData->TILE_SIZE;
            camera->limitTop = 0.0f;
            camera->limitBottom = worldData->worldSizeX * worldData->CHUNK_SIZE * worldData->TILE_SIZE;
        }
    }
}
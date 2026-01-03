#include "interspace/network/NetworkPackets.hpp"
#include "interspace/network/Serializer.hpp"
#include "interspace/server/World.hpp"

namespace Interspace::Server
{
    void World::SendWorldData(ENetPeer* to)
    {
        std::vector<uint8_t> data{WORLD_DATA_PACKET};
        Serializer serializer(data);

        serializer
            << worldData->worldSizeX
            << worldData->worldSizeY
            << worldName;

        server->netInterface->SendToClient(to, data, ENET_PACKET_FLAG_RELIABLE);
    }
}

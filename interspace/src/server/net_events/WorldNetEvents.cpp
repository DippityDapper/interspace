#include <ranges>

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

    void World::BroadcastChunksData()
    {
        for (const auto& chunk : chunks | std::views::values)
        {
            if (chunk->tiles.size() < worldData->CHUNK_SIZE * worldData->CHUNK_SIZE)
                continue;

            std::vector<uint8_t> chunkData{CHUNK_GENERATED_PACKET};
            Serializer serializer(chunkData);

            serializer
                << chunk->data.position.x
                << chunk->data.position.y;

            for (const auto& tile : chunk->tiles | std::views::values)
            {
                serializer << tile->data.tileId << tile->data.variant;
            }

            for (auto factionId : chunk->seenBy)
            {
                Faction* faction = factions[factionId].get();
                for (const auto& memberId : faction->data.members | std::views::keys)
                {
                    ENetPeer* peer = server->GetPeer(memberId);
                    server->netInterface->SendToClient(peer, chunkData, ENET_PACKET_FLAG_RELIABLE);
                }
            }
        }
    }
}

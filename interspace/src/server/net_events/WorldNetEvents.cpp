#include <ranges>

#include "interspace/game/DBHelper.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/network/Serializer.hpp"
#include "interspace/server/World.hpp"
#include "SDL3/SDL_log.h"

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

    void World::SendChunkData(ENetPeer* to, uint32_t playerId)
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

            std::vector<uint16_t> playerFactions = DBHelper::GetPlayerFactionIds(worldName, playerId);

            for (uint16_t factionId : playerFactions)
            {
                if (chunk->seenByFaction.contains(factionId))
                    server->netInterface->SendToClient(to, chunkData, ENET_PACKET_FLAG_RELIABLE);
            }
        }
    }

    void World::BroadcastChunkData(Chunk* chunk)
    {
        if (chunk->tiles.size() < worldData->CHUNK_SIZE * worldData->CHUNK_SIZE)
            return;;

        std::vector<uint8_t> chunkData{CHUNK_GENERATED_PACKET};
        Serializer serializer(chunkData);

        serializer
            << chunk->data.position.x
            << chunk->data.position.y;

        for (const auto& tile : chunk->tiles | std::views::values)
        {
            serializer << tile->data.tileId << tile->data.variant;
        }

        for (const auto& peer : server->GetPeers())
        {
            std::vector<uint16_t> playerFactions = DBHelper::GetPlayerFactionIds(worldName, peer.first);

            for (uint16_t factionId : playerFactions)
            {
                if (chunk->seenByFaction.contains(factionId))
                    server->netInterface->SendToClient(peer.second, chunkData, ENET_PACKET_FLAG_RELIABLE);
            }
        }

    }
}

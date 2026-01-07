#include <random>
#include <ranges>

#include "igneous/Database.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/server/Tiles.hpp"
#include "interspace/server/World.hpp"
#include "SDL3/SDL_log.h"

namespace Interspace::Server
{
    void World::InitFactions()
    {
        SQLite::Database* db = DBHelper::db.get();

        auto factionsQuery = Engine::Database::Query(db, "SELECT * FROM faction WHERE worldId = '" + worldName + "'");
        for (auto factionQuery : factionsQuery)
        {
            uint16_t factionId = std::stoul(factionQuery["factionId"]);
            std::string factionName{factionQuery["factionName"]};
            uint32_t ownerId = 0;
            if (!factionQuery["factionOwner"].empty())
                ownerId = std::stoul(factionQuery["factionOwner"]);

            factions.emplace(factionId, std::make_unique<Faction>());
            Faction* faction = factions[factionId].get();

            faction->data.id = factionId;
            faction->data.name = factionName;
            faction->data.ownerId = ownerId;

            auto factionMembersQuery = Engine::Database::Query(db, "SELECT playerId FROM factionMember WHERE factionId = " + std::to_string(factionId));
            for (auto factionMemberQuery : factionMembersQuery)
            {
                uint32_t playerId = std::stoul(factionMemberQuery["playerId"]);
                std::string playerName = server->GetUsername(playerId);
                faction->data.members.emplace(playerId, playerName);
            }

            auto colonistsQuery = Engine::Database::Query(db,
                "SELECT colonistId, colonistName FROM colonist WHERE factionId = " +
                std::to_string(factionId) + " AND worldId = '" + worldName + "'");
            for (auto colonistQuery : colonistsQuery)
            {
                uint16_t colonistId = std::stoul(colonistQuery["colonistId"]);
                std::string colonistName = colonistQuery["colonistName"];
                faction->AddColonist(colonistName, colonistId, DBHelper::GetColonistPosition(worldName, colonistId));
            }
        }

        if (!DBHelper::FactionExistsByName(worldName, "factionless"))
        {
            std::mt19937 gen(std::random_device{}());
            int factionId = 0;
            do
            {
                std::uniform_int_distribution<> factionIdDist(1, UINT16_MAX);
                factionId = factionIdDist(gen);
            } while (DBHelper::FactionExists(factionId, worldName));

            DBHelper::InsertFaction(factionId, worldName, "factionless", 0);

            uint16_t newFactionId = DBHelper::GetFactionIdByName(worldName, "factionless");
            factions.emplace(newFactionId, std::make_unique<Faction>());
            Faction* faction = factions[newFactionId].get();

            faction->data.ownerId = 0;
            faction->data.id = newFactionId;
            faction->data.name = "factionless";
        }
    }

    uint16_t World::AddFaction(const std::string& factionName, uint32_t ownerId)
    {
        if (DBHelper::FactionExistsByName(worldName, factionName))
        {
            return 0;
        }

        std::mt19937 gen(std::random_device{}());
        int factionId = 0;
        do
        {
            std::uniform_int_distribution<> factionIdDist(1, UINT16_MAX);
            factionId = factionIdDist(gen);
        } while (DBHelper::FactionExists(factionId, worldName));

        DBHelper::InsertFaction(factionId, worldName, factionName, ownerId);

        factions.emplace(factionId, std::make_unique<Faction>());

        Faction* faction = factions[factionId].get();
        faction->data.ownerId = ownerId;
        faction->data.id = factionId;
        faction->data.name = factionName;

        SDL_Log("[Server] Faction (%s [%u]) created.", factionName.c_str(), factionId);

        JoinFaction(factionId, ownerId);
        BroadcastFactionData(factionId);

        uint16_t factionlessId = DBHelper::GetFactionIdByName(worldName, "factionless");
        Faction* factionless = factions[factionlessId].get();
        if (factionless->data.members.contains(ownerId))
        {
            LeaveFaction(factionlessId, ownerId);
        }
        return faction->data.id;
    }

    uint16_t World::AddColonistToFaction(uint16_t factionId, const std::string& colonistName)
    {
        if (!DBHelper::FactionExists(factionId, worldName))
        {
            std::string factionName = factions[factionId]->data.name;
            SDL_Log("[Server] Faction (%s [%u]) not found.", factionName.c_str(), factionId);
            return 0;
        }

        Faction* faction = factions[factionId].get();

        std::mt19937 gen(std::random_device{}());

        float posX = 0;
        float posY = 0;

        if (faction->colonists.empty())
        {
            std::uniform_int_distribution<> posXDist(0, worldData->worldSizeX * worldData->CHUNK_SIZE * worldData->TILE_SIZE);
            posX = (float)posXDist(gen);

            std::uniform_int_distribution<> posYDist(0, worldData->worldSizeY * worldData->CHUNK_SIZE * worldData->TILE_SIZE);
            posY = (float)posYDist(gen);
        }
        else
        {
            Colonist* selectedColonist = nullptr;
            for (const auto& colonist : faction->colonists | std::views::values)
            {
                selectedColonist = colonist.get();
                break;
            }

            if (selectedColonist)
            {
                float selectedX = selectedColonist->entityData.position.x;
                float selectedY = selectedColonist->entityData.position.y;

                float minX = selectedX - 512;
                float minY = selectedY - 512;

                float maxX = selectedX + 512;
                float maxY = selectedY + 512;

                float minWorldX = 0;
                float minWorldY = 0;

                float maxWorldX = worldData->worldSizeX * worldData->CHUNK_SIZE * worldData->TILE_SIZE;
                float maxWorldY = worldData->worldSizeY * worldData->CHUNK_SIZE * worldData->TILE_SIZE;

                minX = std::clamp(minX, minWorldX, maxWorldX);
                minY = std::clamp(minY, minWorldY, maxWorldY);
                maxX = std::clamp(maxX, minWorldX, maxWorldX);
                maxY = std::clamp(maxY, minWorldY, maxWorldY);

                std::uniform_int_distribution<> posXDist(minX, maxX);
                posX = (float)posXDist(gen);

                std::uniform_int_distribution<> posYDist(minY, maxY);
                posY = (float)posYDist(gen);
            }
        }

        int colonistId = 0;
        do
        {
            std::uniform_int_distribution<> colonistIdDist(1, UINT16_MAX);
            colonistId = colonistIdDist(gen);
        } while (DBHelper::ColonistExists(worldName, colonistId));

        DBHelper::InsertColonist(colonistId, worldName, factionId, colonistName, posX, posY);

        faction->AddColonist(colonistName, colonistId, {posX, posY});
        SDL_Log("[Server] Colonist (%s [%u]) created in faction (%s, [%u]).", colonistName.c_str(), colonistId, faction->data.name.c_str(), factionId);
        BroadcastFactionData(factionId);

        return colonistId;
    }

    bool World::DeleteFaction(uint16_t factionId)
    {
        if (!DBHelper::FactionExists(factionId, worldName))
        {
            SDL_Log("[Server] Faction (%u) not found.", factionId);
            return false;
        }

        DBHelper::DeleteFaction(factionId);
        factions.erase(factionId);

        return true;
    }

    bool World::JoinFaction(uint16_t factionId, uint32_t playerId)
    {
        if (!DBHelper::FactionExists(factionId, worldName))
        {
            SDL_Log("[Server] Faction (%u) not found.", factionId);
            return false;
        }

        if (!DBHelper::AddPlayerToFaction(worldName, factionId, playerId))
        {
            return false;
        }

        Faction* faction = factions[factionId].get();
        std::string playerUsername = server->GetUsername(playerId);
        faction->data.members.emplace(playerId, playerUsername);
        BroadcastFactionData(factionId);

        std::string playerName = server->GetUsername(playerId);
        std::string factionName = factions[factionId]->data.name;
        SDL_Log("[Server] Player (%s [%u]) joined faction (%s [%u]).", playerName.c_str(), playerId, factionName.c_str(), factionId);
        return true;
    }

    bool World::JoinFaction(uint32_t playerId)
    {
        if (!DBHelper::IsPlayerInAnyFaction(worldName, playerId))
        {
            uint16_t factionId = DBHelper::GetFactionIdByName(worldName, "factionless");
            JoinFaction(factionId, playerId);
        }
        else
        {
            std::vector<uint16_t> playerFactionsIds = DBHelper::GetPlayerFactionIds(worldName, playerId);
            if (playerFactionsIds.empty())
            {
                std::string playerName = server->GetUsername(playerId);
                SDL_Log("[Server] Player (%s [%u]) not in any factions.", playerName.c_str(), playerId);
                return false;
            }
            uint16_t factionlessId = DBHelper::GetFactionIdByName(worldName, "factionless");

            bool isOnlyFactionless = true;
            bool isInFactionless = false;
            for (auto factionId : playerFactionsIds)
            {
                if (factionId != factionlessId)
                {
                    isOnlyFactionless = false;
                }
                else
                {
                    isInFactionless = true;
                }
                if (!isOnlyFactionless && isInFactionless)
                    break;
            }

            if (!isOnlyFactionless && isInFactionless)
            {
                    LeaveFaction(factionlessId, playerId);
                    playerFactionsIds = DBHelper::GetPlayerFactionIds(worldName, playerId);
            }

            for (auto factionId : playerFactionsIds)
            {
                JoinFaction(factionId, playerId);
            }
        }

        return true;
    }

    bool World::LeaveFaction(uint16_t factionId, uint32_t playerId)
    {
        if (!DBHelper::IsPlayerInFaction(worldName, factionId, playerId))
        {
            std::string playerName = server->GetUsername(playerId);
            std::string factionName = factions[factionId]->data.name;
            SDL_Log("[Server] Player (%s [%u]) not in faction (%s [%u]).", playerName.c_str(), playerId, factionName.c_str(), factionId);
            return false;
        }

        DBHelper::RemovePlayerFromFaction(worldName, factionId, playerId);

        if (!factions.contains(factionId))
            return false;
        Faction* faction = factions[factionId].get();

        if (!faction->data.members.contains(playerId))
            return false;
        faction->data.members.erase(playerId);

        std::string playerName = server->GetUsername(playerId);
        std::string factionName = factions[factionId]->data.name;
        SDL_Log("[Server] Player (%s [%u]) left faction (%s [%u]).", playerName.c_str(), playerId, factionName.c_str(), factionId);
        return true;
    }

    void World::BeginChunkGeneration()
    {
        for (const auto& faction : factions | std::views::values)
        {
            for (const auto& colonist : faction->colonists | std::views::values)
            {
                Engine::Vec2<float> colonistTilePos = colonist->entityData.position / worldData->CHUNK_SIZE;
                uint32_t colonistTileSight = colonist->entityData.sight;

                int32_t sightMinX = colonistTilePos.x - colonistTileSight;
                int32_t sightMinY = colonistTilePos.y - colonistTileSight;
                int32_t sightMaxX = colonistTilePos.x + colonistTileSight;
                int32_t sightMaxY = colonistTilePos.y + colonistTileSight;

                int32_t chunkMinX = sightMinX / worldData->TILE_SIZE;
                int32_t chunkMinY = sightMinY / worldData->TILE_SIZE;
                int32_t chunkMaxX = sightMaxX / worldData->TILE_SIZE;
                int32_t chunkMaxY = sightMaxY / worldData->TILE_SIZE;

                chunkMinX = std::clamp(chunkMinX, 0, (int32_t)worldData->worldSizeX);
                chunkMinY = std::clamp(chunkMinY, 0, (int32_t)worldData->worldSizeY);
                chunkMaxX = std::clamp(chunkMaxX, 0, (int32_t)worldData->worldSizeX);
                chunkMaxY = std::clamp(chunkMaxY, 0, (int32_t)worldData->worldSizeY);

                for (uint16_t chunkX = chunkMinX; chunkX <= chunkMaxX; chunkX++)
                {
                    for (uint16_t chunkY = chunkMinY; chunkY <= chunkMaxY; chunkY++)
                    {
                        Engine::Vec2<uint16_t> chunkPosition{chunkX, chunkY};

                        if (!chunks.contains(chunkPosition))
                        {
                            chunks.emplace(chunkPosition, std::make_unique<Chunk>());
                            Chunk* chunk = chunks[chunkPosition].get();
                            chunk->data.position = chunkPosition;
                            uint32_t tileSeed = seed ^ (chunk->data.position.x * 73856093) ^ (chunk->data.position.y * 19349663);
                            chunk->tileGen.seed(tileSeed);

                            chunkQueue.emplace(chunk);
                            SDL_Log("[Server] Chunk added to queue at (%u, %u).", chunkX, chunkY);
                        }

                        Chunk* chunk = chunks[chunkPosition].get();

                        if (!chunk->seenByFaction.contains(faction->data.id))
                        {
                            chunk->seenByFaction.emplace(faction->data.id);
                            BroadcastChunkData(chunk);
                        }
                    }
                }
            }
        }
    }

    void World::GenerateChunks()
    {
        uint8_t maxChunkPerCycle = 4;
        uint8_t maxChunkIndex = 0;

        for (int i = 0; i < chunkQueue.size(); i++)
        {
            Chunk* chunk = chunkQueue.front();

            for (uint16_t w = chunk->tiles.size(); w < worldData->CHUNK_SIZE * worldData->CHUNK_SIZE; w++)
            {
                uint8_t tileX = w % worldData->CHUNK_SIZE;
                uint8_t tileY = w / worldData->CHUNK_SIZE;
                Engine::Vec2<uint8_t> tilePosition{tileX, tileY};
                chunk->tiles.emplace(tilePosition, Tiles::GetRandomTileBySeed("grass_flower", chunk->tileGen));
            }

            if (chunk->tiles.size() >= worldData->CHUNK_SIZE * worldData->CHUNK_SIZE)
            {
                chunkQueue.pop();
                BroadcastChunkData(chunk);
                SDL_Log("[Server] Chunk generated at (%u, %u).", chunk->data.position.x, chunk->data.position.y);
            }

            maxChunkIndex++;
            if (maxChunkIndex > maxChunkPerCycle)
                break;
        }
    }
}

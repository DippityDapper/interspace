#include <random>
#include <ranges>

#include "igneous/engine/Database.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/server/Tiles.hpp"
#include "interspace/server/World.hpp"
#include "SDL3/SDL_log.h"
#include "interspace/network/NetworkPackets.hpp"

namespace Interspace::Server
{
    void World::InitFactions()
    {
        SQLite::Database* db = DBHelper::worldDb.get();

        SQLite::Statement factionQuery{*db, "SELECT factionId, factionName, factionOwner FROM faction"};
        while (factionQuery.executeStep())
        {
            uint16_t factionId = factionQuery.getColumn(0).getUInt();
            std::string factionName{factionQuery.getColumn(1).getString()};
            uint32_t ownerId = factionQuery.getColumn(2).getUInt();

            factions.emplace(factionId, std::make_unique<Faction>());
            Faction* faction = factions[factionId].get();

            faction->data.id = factionId;
            faction->data.name = factionName;
            faction->data.ownerId = ownerId;

            SQLite::Statement factionMembersQuery{*db, "SELECT playerId FROM factionMember WHERE factionId = ?"};
            factionMembersQuery.bind(1, factionId);

            while (factionMembersQuery.executeStep())
            {
                uint32_t playerId = factionMembersQuery.getColumn(0).getUInt();
                std::string playerName = server->GetUsername(playerId);
                faction->data.members.emplace(playerId, playerName);
            }

            SQLite::Statement colonistsQuery{*db, "SELECT colonistId, colonistName, lastSeenX, lastSeenY FROM colonist WHERE factionId = ?"};
            colonistsQuery.bind(1, factionId);
            while (colonistsQuery.executeStep())
            {
                uint32_t colonistId = colonistsQuery.getColumn(0).getUInt();
                std::string colonistName = colonistsQuery.getColumn(1).getString();
                float colonistX = colonistsQuery.getColumn(2).getDouble();
                float colonistY = colonistsQuery.getColumn(3).getDouble();
                Engine::Vec2<float> colonistPosition{colonistX, colonistY};

                faction->AddColonist(colonistName, colonistId, colonistPosition);
            }
        }

        if (!DBHelper::FactionExists("factionless"))
        {
            std::mt19937 gen(std::random_device{}());
            int factionId = 0;
            do
            {
                std::uniform_int_distribution<> factionIdDist(1, UINT16_MAX);
                factionId = factionIdDist(gen);
            } while (DBHelper::FactionExists(factionId));

            DBHelper::InsertFaction(factionId, "factionless", 0);

            uint16_t newFactionId = DBHelper::GetFactionId("factionless");
            factions.emplace(newFactionId, std::make_unique<Faction>());
            Faction* faction = factions[newFactionId].get();

            faction->data.ownerId = 0;
            faction->data.id = newFactionId;
            faction->data.name = "factionless";
        }
    }

    uint16_t World::AddFaction(const std::string& factionName, uint32_t ownerId)
    {
        if (DBHelper::FactionExists(factionName))
        {
            return 0;
        }

        std::mt19937 gen(std::random_device{}());
        int factionId = 0;
        do
        {
            std::uniform_int_distribution<> factionIdDist(1, UINT16_MAX);
            factionId = factionIdDist(gen);
        } while (DBHelper::FactionExists(factionId));

        DBHelper::InsertFaction(factionId, factionName, ownerId);

        factions.emplace(factionId, std::make_unique<Faction>());

        Faction* faction = factions[factionId].get();
        faction->data.ownerId = ownerId;
        faction->data.id = factionId;
        faction->data.name = factionName;

        SDL_Log("[Server] Faction (%s [%u]) created.", factionName.c_str(), factionId);

        JoinFaction(factionId, ownerId);
        BroadcastFactionData(factionId);

        uint16_t factionlessId = DBHelper::GetFactionId("factionless");
        Faction* factionless = factions[factionlessId].get();
        if (factionless->data.members.contains(ownerId))
        {
            LeaveFaction(factionlessId, ownerId);
        }
        return faction->data.id;
    }

    uint32_t World::AddColonistToFaction(uint16_t factionId, const std::string& colonistName)
    {
        if (!DBHelper::FactionExists(factionId))
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
            std::uniform_int_distribution posXDist(0, worldData->worldSizeX * worldData->CHUNK_SIZE * worldData->TILE_SIZE);
            posX = (float) posXDist(gen);

            std::uniform_int_distribution posYDist(0, worldData->worldSizeY * worldData->CHUNK_SIZE * worldData->TILE_SIZE);
            posY = (float) posYDist(gen);
        }
        else
        {
            Colonist* selectedColonist = nullptr;
            for (const auto& colonist: faction->colonists | std::views::values)
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
                posX = (float) posXDist(gen);

                std::uniform_int_distribution<> posYDist(minY, maxY);
                posY = (float) posYDist(gen);
            }
        }

        int colonistId = 0;
        do
        {
            colonistId = SDL_rand(UINT32_MAX);
        } while (DBHelper::ColonistExists(colonistId));

        DBHelper::InsertColonist(colonistId, factionId, colonistName, posX, posY);

        faction->AddColonist(colonistName, colonistId, {posX, posY});
        SDL_Log("[Server] Colonist (%s [%u]) created in faction (%s, [%u]).", colonistName.c_str(), colonistId, faction->data.name.c_str(), factionId);
        BroadcastFactionData(factionId);

        return colonistId;
    }

    bool World::DeleteFaction(uint16_t factionId)
    {
        if (!DBHelper::FactionExists(factionId))
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
        if (!DBHelper::FactionExists(factionId))
        {
            SDL_Log("[Server] Faction (%u) not found.", factionId);
            return false;
        }

        if (!DBHelper::AddPlayerToFaction(factionId, playerId))
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
        if (!DBHelper::IsPlayerInAnyFaction(playerId))
        {
            uint16_t factionId = DBHelper::GetFactionId("factionless");
            JoinFaction(factionId, playerId);

            SendFactionCreateRequest(playerId);
        }
        else
        {
            std::vector<uint16_t> playerFactionsIds = DBHelper::GetPlayerFactionIds(playerId);
            if (playerFactionsIds.empty())
            {
                std::string playerName = server->GetUsername(playerId);
                SDL_Log("[Server] Player (%s [%u]) not in any factions.", playerName.c_str(), playerId);
                return false;
            }
            uint16_t factionlessId = DBHelper::GetFactionId("factionless");

            bool isOnlyFactionless = true;
            bool isInFactionless = false;
            for (auto factionId: playerFactionsIds)
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
                playerFactionsIds = DBHelper::GetPlayerFactionIds(playerId);
            }

            for (auto factionId: playerFactionsIds)
            {
                JoinFaction(factionId, playerId);
            }
        }

        return true;
    }

    bool World::LeaveFaction(uint16_t factionId, uint32_t playerId)
    {
        if (!DBHelper::IsPlayerInFaction(factionId, playerId))
        {
            std::string playerName = server->GetUsername(playerId);
            std::string factionName = factions[factionId]->data.name;
            SDL_Log("[Server] Player (%s [%u]) not in faction (%s [%u]).", playerName.c_str(), playerId, factionName.c_str(), factionId);
            return false;
        }

        DBHelper::RemovePlayerFromFaction(factionId, playerId);

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
}

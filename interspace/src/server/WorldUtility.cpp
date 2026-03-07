#include <random>
#include <ranges>

#include "igneous/engine/Database.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/server/ServerTiles.hpp"
#include "interspace/server/ServerWorld.hpp"
#include "SDL3/SDL_log.h"
#include "interspace/network/NetworkPackets.hpp"

namespace Interspace::Server
{
    void ServerWorld::InitFactions()
    {
        SQLite::Database* db = DBHelper::worldDb.get();

        SQLite::Statement factionQuery{*db, "SELECT factionId, factionName, factionOwner FROM faction"};
        while (factionQuery.executeStep())
        {
            faction_id_t factionId = factionQuery.getColumn(0).getUInt();
            std::string factionName{factionQuery.getColumn(1).getString()};
            client_id_t ownerId = factionQuery.getColumn(2).getUInt();

            factions.emplace(factionId, std::make_unique<ServerFaction>());
            ServerFaction* faction = factions[factionId].get();

            faction->id = factionId;
            faction->name = factionName;
            faction->ownerId = ownerId;

            SQLite::Statement factionMembersQuery{*db, "SELECT playerId FROM factionMember WHERE factionId = ?"};
            factionMembersQuery.bind(1, factionId);

            while (factionMembersQuery.executeStep())
            {
                client_id_t clientId = factionMembersQuery.getColumn(0).getUInt();
                std::string playerName = server->GetUsername(clientId);
                faction->members.emplace(clientId, playerName);
            }

            SQLite::Statement colonistsQuery{*db, "SELECT colonistId, colonistName, lastSeenX, lastSeenY FROM colonist WHERE factionId = ?"};
            colonistsQuery.bind(1, factionId);
            while (colonistsQuery.executeStep())
            {
                entity_id_t colonistId = colonistsQuery.getColumn(0).getUInt();
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
            faction_id_t factionId = 0;
            do
            {
                std::uniform_int_distribution<faction_id_t> factionIdDist(1, std::numeric_limits<faction_id_t>::max());
                factionId = factionIdDist(gen);
            } while (DBHelper::FactionExists(factionId));

            DBHelper::InsertFaction(factionId, "factionless", 0);

            faction_id_t newFactionId = DBHelper::GetFactionId("factionless");
            factions.emplace(newFactionId, std::make_unique<ServerFaction>());
            ServerFaction* faction = factions[newFactionId].get();

            faction->ownerId = 0;
            faction->id = newFactionId;
            faction->name = "factionless";
        }
    }

    faction_id_t ServerWorld::AddFaction(const std::string& factionName, client_id_t ownerId)
    {
        if (DBHelper::FactionExists(factionName))
        {
            return 0;
        }

        std::mt19937 gen(std::random_device{}());
        faction_id_t factionId = 0;
        do
        {
            std::uniform_int_distribution<faction_id_t> factionIdDist(1, std::numeric_limits<faction_id_t>::max());
            factionId = factionIdDist(gen);
        } while (DBHelper::FactionExists(factionId));

        DBHelper::InsertFaction(factionId, factionName, ownerId);

        factions.emplace(factionId, std::make_unique<ServerFaction>());

        ServerFaction* faction = factions[factionId].get();
        faction->ownerId = ownerId;
        faction->id = factionId;
        faction->name = factionName;

        SDL_Log("[Server] Faction (%s [%u]) created.", factionName.c_str(), factionId);

        JoinFaction(factionId, ownerId);
        BroadcastFactionData(factionId);

        faction_id_t factionlessId = DBHelper::GetFactionId("factionless");
        ServerFaction* factionless = factions[factionlessId].get();
        if (factionless->members.contains(ownerId))
        {
            LeaveFaction(factionlessId, ownerId);
        }
        return faction->id;
    }

    entity_id_t ServerWorld::AddColonistToFaction(faction_id_t factionId, const std::string& colonistName)
    {
        if (!DBHelper::FactionExists(factionId))
        {
            std::string factionName = factions[factionId]->name;
            SDL_Log("[Server] Faction (%s [%u]) not found.", factionName.c_str(), factionId);
            return 0;
        }

        ServerFaction* faction = factions[factionId].get();

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
            ServerColonist* selectedColonist = nullptr;
            for (const auto& colonist: faction->colonists | std::views::values)
            {
                selectedColonist = colonist.get();
                break;
            }

            if (selectedColonist)
            {
                float selectedX = selectedColonist->position.x;
                float selectedY = selectedColonist->position.y;

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

                std::uniform_int_distribution<uint32_t> posXDist(minX, maxX);
                posX = (float) posXDist(gen);

                std::uniform_int_distribution<uint32_t> posYDist(minY, maxY);
                posY = (float) posYDist(gen);
            }
        }

        entity_id_t colonistId = 0;
        do
        {
            std::uniform_int_distribution<entity_id_t> colonistIdDist(1, std::numeric_limits<entity_id_t>::max());
            colonistId = colonistIdDist(gen);
        } while (DBHelper::ColonistExists(colonistId));

        DBHelper::InsertColonist(colonistId, factionId, colonistName, posX, posY);

        faction->AddColonist(colonistName, colonistId, {posX, posY});
        SDL_Log("[Server] Colonist (%s [%u]) created in faction (%s, [%u]).", colonistName.c_str(), colonistId, faction->name.c_str(), factionId);
        BroadcastFactionData(factionId);

        return colonistId;
    }

    bool ServerWorld::DeleteFaction(faction_id_t factionId)
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

    bool ServerWorld::JoinFaction(faction_id_t factionId, client_id_t clientId)
    {
        if (!DBHelper::FactionExists(factionId))
        {
            SDL_Log("[Server] Faction (%u) not found.", factionId);
            return false;
        }

        if (!DBHelper::AddPlayerToFaction(factionId, clientId))
        {
            return false;
        }

        ServerFaction* faction = factions[factionId].get();
        std::string playerUsername = server->GetUsername(clientId);
        faction->members.emplace(clientId, playerUsername);
        BroadcastFactionData(factionId);

        std::string playerName = server->GetUsername(clientId);
        std::string factionName = factions[factionId]->name;
        SDL_Log("[Server] Player (%s [%u]) joined faction (%s [%u]).", playerName.c_str(), clientId, factionName.c_str(), factionId);
        return true;
    }

    bool ServerWorld::JoinFaction(client_id_t clientId)
    {
        if (!DBHelper::IsPlayerInAnyFaction(clientId))
        {
            faction_id_t factionId = DBHelper::GetFactionId("factionless");
            JoinFaction(factionId, clientId);

            SendFactionCreateRequest(clientId);
        }
        else
        {
            std::vector<faction_id_t> playerFactionsIds = DBHelper::GetPlayerFactionIds(clientId);
            if (playerFactionsIds.empty())
            {
                std::string playerName = server->GetUsername(clientId);
                SDL_Log("[Server] Player (%s [%u]) not in any factions.", playerName.c_str(), clientId);
                return false;
            }
            faction_id_t factionlessId = DBHelper::GetFactionId("factionless");

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
                LeaveFaction(factionlessId, clientId);
                playerFactionsIds = DBHelper::GetPlayerFactionIds(clientId);
            }
            else if (isOnlyFactionless && isInFactionless)
            {
                SendFactionCreateRequest(clientId);
            }

            for (auto factionId: playerFactionsIds)
            {
                JoinFaction(factionId, clientId);
            }
        }

        return true;
    }

    bool ServerWorld::LeaveFaction(faction_id_t factionId, client_id_t clientId)
    {
        if (!DBHelper::IsPlayerInFaction(factionId, clientId))
        {
            std::string playerName = server->GetUsername(clientId);
            std::string factionName = factions[factionId]->name;
            SDL_Log("[Server] Player (%s [%u]) not in faction (%s [%u]).", playerName.c_str(), clientId, factionName.c_str(), factionId);
            return false;
        }

        DBHelper::RemovePlayerFromFaction(factionId, clientId);

        if (!factions.contains(factionId))
            return false;
        ServerFaction* faction = factions[factionId].get();

        if (!faction->members.contains(clientId))
            return false;
        faction->members.erase(clientId);

        std::string playerName = server->GetUsername(clientId);
        std::string factionName = factions[factionId]->name;
        SDL_Log("[Server] Player (%s [%u]) left faction (%s [%u]).", playerName.c_str(), clientId, factionName.c_str(), factionId);
        return true;
    }
}

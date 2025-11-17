#include "game/server/WorldServer.hpp"

#include <cstring>

#include "SDL3/SDL_log.h"

#include "dapper2d/CFGParser.hpp"

#include "game/game/Game.hpp"

namespace Game
{
    WorldServer::WorldServer(Server* _server, const std::string& worldName)
    {
        server = _server;

        name = worldName;
        Engine::CFGParser::LoadConfig("worlds/" + worldName + "/configs.cfg", name);
    }

    void WorldServer::Init()
    {
        ConnectNetEvents();

        try
        {
            seed = Engine::CFGParser::GetUInt32(name, "world_seed");
            worldSizeX = Engine::CFGParser::GetUInt16(name, "world_size_x");
            worldSizeY = Engine::CFGParser::GetUInt16(name, "world_size_y");
            // Area::AREA_SIZE = Engine::CFGParser::GetInt(name, "area_size");
            // Tile::TILE_SIZE = Engine::CFGParser::GetInt(name, "tile_size");
        }
        catch (const std::exception& e)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", e.what());
            Game::Disconnect();
            return;
        }
    }

    void WorldServer::Update(float delta)
    {
    }

    void WorldServer::HandleEvents(SDL_Event& event)
    {
    }

    void WorldServer::Clean()
    {
    }

    bool WorldServer::AddPlayer(uint32_t clientId, const Engine::Vec2<uint64_t>& position)
    {
        if (players.contains(clientId))
            return false;

        players.emplace(clientId, std::make_unique<PlayerServer>(clientId, position));
        SDL_Log("[Server] Player created: %u", clientId);
        return true;
    }

    bool WorldServer::RemovePlayer(uint32_t clientId)
    {
        if (!players.contains(clientId))
            return false;
        players.erase(clientId);
        return true;
    }

    uint32_t WorldServer::AddFaction(uint32_t ownerId, const std::string& factionName)
    {
        uint32_t factionId = nextFactionId++;
        factions.emplace(factionId, std::make_unique<FactionServer>(factionId, ownerId, factionName));
        factionIdToName.emplace(factionId, factionName);
        factionNameToId.emplace(factionName, factionId);
        factionOwnerToId.emplace(ownerId, factionId);

        SDL_Log("[Server] Faction created: %s [%u]", factionName.c_str(), factionId);
        for (const auto& kvp : factions[factionId]->colonists)
        {
            players[ownerId]->selectedColonists.emplace(kvp.first, kvp.second.get());
        }

        return factionId;
    }

    bool WorldServer::RemoveFaction(uint32_t factionId)
    {
        if (!factions.contains(factionId))
            return false;

        factions.erase(factionId);

        if (!factionIdToName.contains(factionId))
            return false;

        std::string factionName = factionIdToName[factionId];
        if (!factionNameToId.contains(factionName))
        {
            factionIdToName.erase(factionId);
            return false;
        }

        uint32_t ownerId = factionNameToId[factionName];
        if (!factionOwnerToId.contains(ownerId))
        {
            factionNameToId.erase(factionName);
            factionIdToName.erase(factionId);
            return false;
        }

        factionOwnerToId.erase(ownerId);
        factionNameToId.erase(factionName);
        factionIdToName.erase(factionId);
        return true;
    }

    FactionServer* WorldServer::GetFaction(uint32_t factionId)
    {
        if (!factions.contains(factionId))
            return nullptr;
        return factions[factionId].get();
    }
}

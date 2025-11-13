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
        server->ConnectToEvent(WORLD_DATA_REQUEST, this, &WorldServer::OnWorldDataRequest);
        server->ConnectToEvent(AREA_DATA_REQUEST, this, &WorldServer::OnAreaDataRequest);

        server->ConnectToEvent(CLIENT_CONNECTED, this, &WorldServer::OnClientConnected);
        server->ConnectToEvent(CLIENT_DISCONNECTED, this, &WorldServer::OnClientDisconnected);

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

    uint32_t WorldServer::AddFaction(uint32_t ownerId, const std::string& factionName)
    {
        uint32_t factionId = nextFactionId++;
        factions.emplace(factionId, std::make_unique<FactionServer>(factionId, ownerId, factionName));
        factionIdToName.emplace(factionId, factionName);
        factionNameToId.emplace(factionName, factionId);
        factionOwnerToId.emplace(ownerId, factionId);

        SDL_Log("[Server] Faction created: %s [%u]", factionName.c_str(), factionId);

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

    void WorldServer::OnWorldDataRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        std::vector<uint8_t> worldData;
        worldData.push_back(WORLD_DATA_PACKET);

        uint32_t worldNameLen = static_cast<uint32_t>(name.size());
        PackBytes(worldData, &worldNameLen, sizeof(uint32_t));
        PackBytes(worldData, name.data(), worldNameLen);
        PackBytes(worldData, &worldSizeX, sizeof(uint16_t));
        PackBytes(worldData, &worldSizeY, sizeof(uint16_t));

        server->netInterface->SendToClient(from, worldData);
    }

    void WorldServer::OnAreaDataRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        size_t offset = 1;

        uint16_t requestedAreaX = UnpackUint16(data, offset);
        uint16_t requestedAreaY = UnpackUint16(data, offset);
        Engine::Vec2<uint16_t> requestedAreaPosition(requestedAreaX, requestedAreaY);

        AreaServer* area = new AreaServer(requestedAreaPosition, seed);
        if (!areas.contains(requestedAreaPosition))
        {
            area->Create();
            area->generationComplete = true;
        }
        else
        {
            area = areas[requestedAreaPosition].get();
        }

        std::vector<uint8_t> areaData = area->Serialize();

        std::vector<uint8_t> response;
        response.push_back(AREA_DATA_PACKET);
        response.insert(response.end(), areaData.begin(), areaData.end());
        server->netInterface->SendToClient(from, response);

        delete area;
    }

    void WorldServer::OnClientConnected(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        size_t offset = 1;

        uint32_t peerId = UnpackUint32(data, offset);
        uint32_t usernameLength = UnpackUint32(data, offset);
        std::string peerUsername = UnpackString(data, offset, usernameLength);

        AddFaction(peerId, peerUsername);
    }

    void WorldServer::OnClientDisconnected(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        size_t offset = 1;

        uint32_t peerId = UnpackUint32(data, offset);
        std::string peerUsername = server->GetUsername(peerId);

        RemoveFaction(factionOwnerToId[peerId]);
    }
}

#include "game/world/WorldServer.hpp"

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

    void WorldServer::OnWorldDataRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        std::vector<uint8_t> response;
        response.push_back(WORLD_DATA_PACKET);

        uint32_t worldNameLen = static_cast<uint32_t>(name.size());
        uint8_t* worldNameLenBytes = reinterpret_cast<uint8_t*>(&worldNameLen);
        response.insert(response.end(), worldNameLenBytes, worldNameLenBytes + sizeof(uint32_t));
        response.insert(response.end(), name.begin(), name.end());

        uint8_t* worldSizeXBytes = reinterpret_cast<uint8_t*>(&worldSizeX);
        response.insert(response.end(), worldSizeXBytes, worldSizeXBytes + sizeof(uint16_t));
        uint8_t* worldSizeYBytes = reinterpret_cast<uint8_t*>(&worldSizeY);
        response.insert(response.end(), worldSizeYBytes, worldSizeYBytes + sizeof(uint16_t));

        server->netInterface->SendToClient(from, response);
    }

    void WorldServer::OnAreaDataRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        uint32_t offset = 1;

        uint16_t requestedAreaX = 0;
        memcpy(&requestedAreaX, &data[offset], sizeof(uint16_t));
        offset += sizeof(uint16_t);

        uint16_t requestedAreaY = 0;
        memcpy(&requestedAreaY, &data[offset], sizeof(uint16_t));

        Engine::Vec2<uint16_t> requestedAreaPosition(requestedAreaX, requestedAreaY);

        AreaServer* area = new AreaServer(requestedAreaPosition, seed);
        if (!areas.contains(requestedAreaPosition))
        {
            // areas[requestedAreaPosition] = std::make_unique<AreaServer>(requestedAreaPosition, seed);
            // area = areas[requestedAreaPosition].get();
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
}

#include "server/WorldData.hpp"

#include "dapper2d/CFGParser.hpp"
#include "SDL3/SDL_log.h"

#include "server/Server.hpp"

namespace Server
{
    uint32_t WorldData::worldSeed = 0;
    int WorldData::WORLD_SIZE_X = 0;
    int WorldData::WORLD_SIZE_Y = 0;
    int WorldData::REGION_SIZE = 0;

    WorldData::WorldData(const std::string& _name)
    {
        name = _name;

        std::string cfgPath = Server::worldsPathAbs + "/" + name + "/configs.cfg";
        Engine::CFGParser::LoadConfig(cfgPath, name);

        try
        {
            worldSeed = Engine::CFGParser::GetUInt32(name, "world_seed");
            WORLD_SIZE_X = Engine::CFGParser::GetInt(name, "world_size_x");
            WORLD_SIZE_Y = Engine::CFGParser::GetInt(name, "world_size_y");
            REGION_SIZE = Engine::CFGParser::GetInt(name, "region_size");
            // Area::AREA_SIZE = Engine::CFGParser::GetInt(name, "area_size");
            // Tile::TILE_SIZE = Engine::CFGParser::GetInt(name, "tile_size");
        }
        catch (const std::exception& e)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", e.what());
            Server::Stop();
            return;
        }

        SDL_Log("World Seed: %d", worldSeed);
        SDL_Log("World Size X: %d", WORLD_SIZE_X);
        SDL_Log("World Size Y: %d", WORLD_SIZE_Y);
        SDL_Log("Region Size: %d", REGION_SIZE);
    }

}
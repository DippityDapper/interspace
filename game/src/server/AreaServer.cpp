#include "game/server/AreaServer.hpp"

#include <filesystem>
#include <fstream>

#include "game/server/TileRegistryServer.hpp"
#include "game/server/WorldServer.hpp"
#include "SDL3/SDL_log.h"

namespace Game
{
    AreaServer::AreaServer(Engine::Vec2<uint16_t> _position, uint32_t worldSeed)
    {
        position = _position;
        seed = worldSeed ^ (position.x * 73856093) ^ (position.y * 19349663);
        seedGen.seed(seed);
    }

    void AreaServer::Init()
    {
    }

    void AreaServer::Clean()
    {

    }

    bool AreaServer::Create()
    {
        if (Load())
            return true;

        if (Generate())
            return true;

        return false;
    }

    void AreaServer::Update(float delta)
    {

    }


    bool AreaServer::Load()
    {
        uint8_t areaSize = WorldServer::AREA_SIZE;
        uint8_t regionSize = WorldServer::REGION_SIZE;

        uint8_t rx = areaSize / regionSize;
        uint8_t ry = areaSize / regionSize;

        std::string worldName = WorldServer::name;
        std::string fileName = "region_" + std::to_string(rx) + "_" + std::to_string(ry) + ".data";
        std::string filePath = "worlds/" + worldName + "/regions/" + fileName;

        if (!std::filesystem::exists(filePath))
            return false;

        std::ifstream file(filePath, std::ios::in | std::ios::binary);
        if (!file.is_open())
            return false;

        while (file.peek() != EOF)
        {
            uint16_t saveAreaX = 0;
            uint16_t saveAreaY = 0;
            file.read(reinterpret_cast<char*>(&saveAreaX), sizeof(uint16_t));
            file.read(reinterpret_cast<char*>(&saveAreaY), sizeof(uint16_t));

            uint8_t tileCount = 0;
            file.read(reinterpret_cast<char*>(&tileCount), sizeof(uint32_t));

            if (saveAreaX == position.x && saveAreaY == position.y)
            {
                for (uint32_t i = 0; i < tileCount; i++)
                {
                    uint8_t tileX = 0;
                    uint8_t tileY = 0;
                    TileType tileType = GRASS_1;

                    file.read(reinterpret_cast<char*>(&tileX), sizeof(uint8_t));
                    file.read(reinterpret_cast<char*>(&tileY), sizeof(uint8_t));
                    file.read(reinterpret_cast<char*>(&tileType), sizeof(uint8_t));

                    Engine::Vec2<uint8_t> tilePosition{tileX, tileY};
                    tiles[tilePosition] = TileRegistryServer::GetTile(tileType);
                }
                file.close();
                return true;
            }

            file.seekg(tileCount * (sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t)), std::ios::cur);
        }

        file.close();
        return false;
    }

    bool AreaServer::Generate()
    {
        uint8_t areaSize = WorldServer::AREA_SIZE;

        for (uint8_t x = 0; x < areaSize; x++)
        {
            for (uint8_t y = 0; y < areaSize; y++)
            {
                std::uniform_int_distribution<> flowerChance(0, 9);
                bool isFlower = flowerChance(seedGen) == 0;

                TileType tileType = GRASS_1;
                Engine::Vec2<uint8_t> tilePosition{x, y};
                if (isFlower)
                {
                    std::uniform_int_distribution<> flowerTileDistribution(FLOWER_1, FLOWER_16);
                    tileType = (TileType)flowerTileDistribution(seedGen);
                }
                else
                {
                    std::uniform_int_distribution<> grassTileDistribution(GRASS_1, GRASS_16);
                    tileType = (TileType)grassTileDistribution(seedGen);
                }

                tiles[tilePosition] = TileRegistryServer::GetTile(tileType);
            }
        }

        return true;
    }

    std::vector<uint8_t> AreaServer::Serialize()
    {
        std::vector<uint8_t> data;
        uint8_t* positionXBytes = reinterpret_cast<uint8_t*>(&position.x);
        data.insert(data.end(), positionXBytes, positionXBytes + sizeof(uint16_t));

        uint8_t* positionYBytes = reinterpret_cast<uint8_t*>(&position.y);
        data.insert(data.end(), positionYBytes, positionYBytes + sizeof(uint16_t));

        uint32_t tileCount = tiles.size();
        uint8_t* tileCountBytes = reinterpret_cast<uint8_t*>(&tileCount);
        data.insert(data.end(), tileCountBytes, tileCountBytes + sizeof(uint32_t));

        for (const auto& tile : tiles)
        {
            std::vector<uint8_t> tileData = tile.second->Serialize();
            data.insert(data.end(), tileData.begin(), tileData.end());
        }

        return data;
    }
}

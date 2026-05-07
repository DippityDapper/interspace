#pragma once
#include "SQLiteCpp/Database.h"
#include "interspace/shared/world/TileData.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Interspace
{
    class TileManager
    {
      private:
        static inline std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::unique_ptr<TileData>>> tiles{};
        static inline std::unordered_map<std::string, uint32_t> tileNameToId{};
        static inline std::unordered_map<std::string, std::vector<uint32_t>> tileVariantsByName{};

      public:
        static void Init();
        static void CreateTables();

        static bool TileDataExists(const std::string& tileName);
        static bool InsertTileData(uint32_t tileId, uint32_t tileVariant, const std::string& tileName, bool walkable, const std::string& texturePath, uint32_t atlasW, uint32_t atlasH, uint32_t atlasX, uint32_t atlasY);
        static bool DeleteTileData(uint32_t tileId);

        static void AddTileData(uint32_t tileId, uint32_t tileVariant, const std::string& tileName, bool walkable, const std::string& texturePath, uint32_t atlasW, uint32_t atlasH, uint32_t atlasX, uint32_t atlasY);
        static void RemoveTileData(uint32_t tileId, const std::string& tileName);

        static bool TileExists(uint32_t tileId);
        static bool TileVariantExists(uint32_t tileId, uint32_t tileVariant);

        static uint32_t GetTileId(const std::string& tileName);
        static TileData* GetTile(uint32_t tileId, uint32_t tileVariant);
        static std::vector<uint32_t> GetTileVariants(const std::string& tileName);

        static TileData* GetTileOfType(const std::string& tileName, uint32_t tileVariant);
        static TileData* GetTileOfType(uint32_t tileId, uint32_t tileVariant);
        static TileData* GetRandomTileBySeed(const std::string& tileName, uint32_t worldSeed, uint16_t chunkX, uint16_t chunkY);
    };
}
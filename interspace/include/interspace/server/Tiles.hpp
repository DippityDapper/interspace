#pragma once

#include "SQLiteCpp/Database.h"

#include <cstdint>
#include <memory>
#include <random>
#include <unordered_map>
#include <vector>

#include "interspace/server/Tile.hpp"

namespace Interspace::Server
{
    class Tiles
    {
      private:
        static inline std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::unique_ptr<Tile>>> tiles{};
        static inline std::unordered_map<std::string, uint32_t> tileNameToId{};
        static inline std::unordered_map<std::string, std::vector<uint32_t>> tileVariantsByName{};

      public:
        static void Init();
        static Tile* GetRandomTileBySeed(const std::string& tileName, std::mt19937& tileGen);
        static Tile* GetTileOfType(const std::string& tileName, uint32_t tileVariant);
        static Tile* GetTileOfType(uint32_t tileId, uint32_t tileVariant);
    };
}

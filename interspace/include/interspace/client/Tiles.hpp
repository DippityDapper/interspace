#pragma once

#include <unordered_map>
#include <vector>

#include "interspace/client/Tile.hpp"

namespace Interspace::Client
{
    class Tiles
    {
    private:
        static inline std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::unique_ptr<Tile>>> tiles{};
        static inline std::unordered_map<std::string, uint32_t> tileNameToId{};
        static inline std::unordered_map<std::string, std::vector<uint32_t>> tileVariantsByName{};

    public:
        static void Init();
        static Tile* GetRandomTileOfType(const std::string& tileName);
        static Tile* GetTileOfType(const std::string& tileName, uint32_t tileVariant);
        static Tile* GetTileOfType(uint32_t tileId, uint32_t tileVariant);
    };
}

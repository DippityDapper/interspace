#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "interspace/server/Tile.hpp"

namespace Interspace::Server
{
    class Tiles
    {
    private:
        static inline std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::unique_ptr<Tile>>> tiles{};

    public:
        static void Init();
        static Tile* GetRandomTileOfType(const std::string& tileName);
        static Tile* GetTileOfType(const std::string& tileName, uint32_t tileVariant);
        Tile* GetTileOfType(uint32_t tileId, uint32_t tileVariant);
    };
}

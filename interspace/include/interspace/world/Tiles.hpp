#pragma once

#include "interspace/world/Chunk.hpp"
#include "interspace/game/DBHelper.hpp"

#include <unordered_map>
#include <vector>
#include <memory>

namespace Interspace
{
    template<typename TileType>
    requires ExtendsTile<TileType>
    class Tiles
    {
      protected:
        static inline std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::unique_ptr<TileType>>> tiles{};
        static inline std::unordered_map<std::string, uint32_t> tileNameToId{};
        static inline std::unordered_map<std::string, std::vector<uint32_t>> tileVariantsByName{};

      public:
        virtual void Init() = 0;

        TileType* GetTileOfType(const std::string& tileName, uint32_t tileVariant)
        {
            if (!tileNameToId.contains(tileName))
            {
                if (!DBHelper::TileDataExists(tileName))
                {
                    return nullptr;
                }
                tileNameToId.emplace(tileName, DBHelper::GetTileDataId(tileName));
            }

            uint32_t tileId = tileNameToId[tileName];

            if (!tiles.contains(tileId))
                return nullptr;
            if (!tiles[tileId].contains(tileVariant))
                return nullptr;

            TileType* tile = tiles[tileId][tileVariant].get();
            return tile;
        }

        TileType* GetTileOfType(uint32_t tileId, uint32_t tileVariant)
        {
            if (!tiles.contains(tileId))
                return nullptr;
            if (!tiles[tileId].contains(tileVariant))
                return nullptr;

            TileType* tile = tiles[tileId][tileVariant].get();

            return tile;
        }

        virtual ~Tiles() = default;
    };
}
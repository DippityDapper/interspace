#pragma once
#include "interspace/world/TileData.hpp"

#include <memory>

namespace Interspace
{
    struct ClientTile
    {
        std::unique_ptr<TileData> tileData = nullptr;
    };
}
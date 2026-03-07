#pragma once

#include "SQLiteCpp/Database.h"

#include <random>

#include "interspace/server/ServerTile.hpp"
#include "interspace/world/Tiles.hpp"

namespace Interspace::Server
{
    class ServerTiles : public Tiles<ServerTile>
    {
      public:
        void Init() override;
        ServerTile* GetRandomTileBySeed(const std::string& tileName, std::mt19937& tileGen);
    };
}

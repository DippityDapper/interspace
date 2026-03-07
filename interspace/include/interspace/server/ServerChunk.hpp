#pragma once

#include <random>
#include <set>

#include "interspace/server/ServerTile.hpp"
#include "interspace/world/Chunk.hpp"
#include "interspace/game/Typedefs.hpp"

namespace Interspace::Server
{
    class ServerChunk : public Chunk<ServerTile>
    {
      public:
        std::mt19937 tileGen{};
        std::set<faction_id_t> seenByFaction{};
    };
}

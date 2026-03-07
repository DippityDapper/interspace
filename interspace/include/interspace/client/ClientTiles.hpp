#pragma once

#include "interspace/client/ClientTile.hpp"
#include "interspace/world/Tiles.hpp"

namespace Interspace::Client
{
    class ClientTiles : public Tiles<ClientTile>
    {
      public:
        void Init() override;
    };
}

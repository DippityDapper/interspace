#include "game/world/TileServer.hpp"

namespace Game
{
    TileServer::TileServer(TileType _type)
    {
        type = _type;
    }

    std::vector<uint8_t> TileServer::Serialize()
    {
        std::vector<uint8_t> data;
        data.push_back(type);

        return data;
    }
}

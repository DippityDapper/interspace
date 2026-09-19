#pragma once
#include "interspace/shared/tiles/TileState.hpp"

#include <string>
#include <vector>

namespace Interspace
{
    struct TileDefinition
    {
        std::string id;
        std::string name;
        std::string handlerId;
        std::string spriteSheet;
        std::vector<TileState> states;
        std::unordered_map<std::string, nlohmann::json> properties;
        bool hasTileEntity = false;
    };
}

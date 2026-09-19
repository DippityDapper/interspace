#pragma once

#include "nlohmann/json_fwd.hpp"

#include <string>
#include <unordered_map>

namespace Interspace
{
    struct TileState
    {
        std::string id{};
        int frame = 0;
        std::pmr::unordered_map<std::string, nlohmann::json> propertyOverrides{};
    };
}

#pragma once

#include <cstdint>
#include <map>

#include "game/client/ColonistClient.hpp"
#include "dapper2d/Vec2.hpp"

namespace Game
{
    class PlayerClient
    {
    public:
        uint32_t clientId = 0;
        Engine::Vec2<uint64_t> position{};

        std::map<uint32_t, ColonistClient*> selectedColonists{};

    public:
        PlayerClient(uint32_t _clientId, Engine::Vec2<uint64_t> _position);
    };
}

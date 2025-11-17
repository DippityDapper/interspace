#pragma once

#include <cstdint>
#include <map>

#include "game/server/ColonistServer.hpp"
#include "dapper2d/Vec2.hpp"

namespace Game
{
    class PlayerServer
    {
    public:
        uint32_t clientId = 0;
        Engine::Vec2<uint64_t> position{};

        std::map<uint32_t, ColonistServer*> selectedColonists{};

    public:
        PlayerServer(uint32_t _clientId, Engine::Vec2<uint64_t> _position);
    };
}

#pragma once

#include <vector>
#include <cstdint>

#include "enet/enet.h"

#include "dapper2d/Event.hpp"

namespace Game
{
    class ServerNetEvent : public Engine::Event<void, const std::vector<uint8_t>&, ENetPeer*>
    {
    public:
        using Base = Event;

        using Base::Base;
        using Base::Connect;
        using Base::Emit;
        using Base::Disconnect;
        using Base::DisconnectAll;
        using Base::operator();
    };
}

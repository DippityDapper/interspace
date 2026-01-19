#pragma once

#include <vector>
#include <cstdint>

#include "enet/enet.h"

#include "igneous/engine/Event.hpp"

namespace Interspace::Server
{
    class ServerNetEvent : public Engine::Event<void, const std::vector<uint8_t>&, ENetPeer*>
    {
      public:
        using Base = Event;

        using Base::Base;
        using Base::Connect;
        using Base::Disconnect;
        using Base::DisconnectAll;
        using Base::Emit;
        using Base::operator();
    };
}

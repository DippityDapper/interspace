#pragma once

#include <vector>

#include "igneous/Event.hpp"

namespace Interspace::Client
{
    class ClientNetEvent : public Engine::Event<void, const std::vector<uint8_t>&>
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

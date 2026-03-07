#pragma once

#include "interspace/server/ServerEntity.hpp"
#include "interspace/world/Colonist.hpp"

namespace Interspace::Server
{
    class ServerColonist : public Colonist<ServerEntity>
    {
    };
}

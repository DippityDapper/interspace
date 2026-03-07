#pragma once

#include "interspace/client/ClientColonist.hpp"
#include "interspace/world/Faction.hpp"

namespace Interspace::Client
{
    class ClientFaction : public Faction<ClientColonist>
    {
      public:
        ClientColonist* GetColonist(entity_id_t colonistId);
    };
}

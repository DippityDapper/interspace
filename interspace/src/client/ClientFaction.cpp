#include "interspace/client/ClientFaction.hpp"

namespace Interspace::Client
{
    ClientColonist* ClientFaction::GetColonist(entity_id_t colonistId)
    {
        if (!colonists.contains(colonistId))
            return nullptr;
        return colonists[colonistId].get();
    }
}

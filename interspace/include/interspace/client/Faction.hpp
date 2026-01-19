#pragma once

#include "interspace/client/Colonist.hpp"
#include "interspace/world/FactionData.hpp"

namespace Interspace::Client
{
    class Faction
    {
      public:
        FactionData data{};
        std::map<uint16_t, std::unique_ptr<Colonist>> colonists{};

      public:
        void Update(float delta);

        Colonist* GetColonist(uint16_t colonistId);
    };
}

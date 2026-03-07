#pragma once

#include "interspace/world/Entity.hpp"

namespace Interspace::Client
{
    class ClientEntity : public Entity
    {
      public:
        std::string selectedBy{};
    };
}

#pragma once
#include "interspace/game/Typedefs.hpp"
#include "interspace/world/Entity.hpp"

namespace Interspace
{
    template<typename T>
    concept ExtendsEntity = std::is_base_of_v<Entity, T>;

    template<typename EntityType>
    requires ExtendsEntity<EntityType>
    class Colonist : public EntityType
    {
      public:
        using entity_type = EntityType;

        client_id_t selectedBy = 0;
    };
}
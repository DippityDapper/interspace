#pragma once

#include "interspace/world/Colonist.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <memory>

namespace Interspace
{
    template<typename C>
    concept ExtendsColonist = requires {
        typename C::entity_type;
    } && std::is_base_of_v<Colonist<typename C::entity_type>, C>;

    template<typename ColonistType>
    requires ExtendsColonist<ColonistType>
    class Faction
    {
      public:
        using EntityType = ColonistType::entity_type;

      public:
        faction_id_t id = 0;
        std::string name;
        client_id_t ownerId = 0;
        std::map<client_id_t, std::string> members{};
        std::map<entity_id_t, std::unique_ptr<ColonistType>> colonists{};
    };
}
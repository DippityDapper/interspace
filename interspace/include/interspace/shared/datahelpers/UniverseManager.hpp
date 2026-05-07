#pragma once
#include "SQLiteCpp/Database.h"
#include "interspace/shared/game/Typedefs.hpp"
#include "interspace/shared/world/WorldData.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Interspace
{
    class UniverseManager
    {
      private:
        static inline std::unordered_map<std::string, universe_id_t> universeNameToId{};
        static inline std::unordered_map<universe_id_t, std::string> universeIdToName{};

      public:
        static void Init();
        static void CreateTables();

        static bool InsertUniverse(const std::string& universeName, uint32_t seed);
        static bool DeleteUniverse(const std::string& universeName);

        static void AddUniverse(const std::string& universeName, universe_id_t universeId);
        static void RemoveUniverse(universe_id_t universeId);

        static std::unordered_map<universe_id_t, std::string>& GetUniverses();
        static universe_id_t GetUniverseId(const std::string& universeName);
        static std::string GetUniverseName(universe_id_t universeId);
        static uint32_t GetUniverseSeed(universe_id_t universeId);
    };
}
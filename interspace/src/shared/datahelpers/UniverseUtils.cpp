#include "interspace/shared/datahelpers/UniverseUtils.hpp"

#include "interspace/shared/datahelpers/DatabaseManager.hpp"

#include <chrono>
#include <format>
#include <random>

namespace Interspace
{

    void UniverseUtils::Init()
    {
        if (!std::filesystem::exists("data/server"))
            std::filesystem::create_directory("data/server");
        if (!std::filesystem::exists("data/server/universes"))
            std::filesystem::create_directory("data/server/universes");

        if (!std::filesystem::exists("data/client"))
            std::filesystem::create_directory("data/client");
        if (!std::filesystem::exists("data/client/universes"))
            std::filesystem::create_directory("data/client/universes");

        CreateTables();

        SQLite::Database* db = DatabaseManager::GetSharedDatabase();
        SQLite::Statement statement(*db, R"(
            SELECT universeId, universeName FROM universe
        )");

        while (statement.executeStep())
        {
            universe_id_t universeId = statement.getColumn(0).getInt();
            std::string universeName = statement.getColumn(1).getString();
            universeIdToName.emplace(universeId, universeName);
            universeNameToId.emplace(universeName, universeId);
        }
    }

    void UniverseUtils::CreateTables()
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        db->exec(R"(
            CREATE TABLE IF NOT EXISTS universe(
                universeId INTEGER PRIMARY KEY,
                universeName VARCHAR(255) NOT NULL,
                universeSeed INTEGER NOT NULL,
                createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                lastPlayed INTEGER NOT NULL DEFAULT (0),
                gameVersion VARCHAR(50)
            );
        )");
    }

    bool UniverseUtils::InsertUniverse(const std::string& universeName, uint32_t seed)
    {
        std::string serverUniverseDirPath{"data/server/universes/" + universeName};
        std::filesystem::create_directory(serverUniverseDirPath);
        std::string clientUniverseDirPath{"data/client/universes/" + universeName};
        std::filesystem::create_directory(clientUniverseDirPath);

        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement statement(*db, R"(
            INSERT INTO universe(universeId, universeName, universeSeed, gameVersion)
            VALUES(?, ?, ?, ?)
        )");

        const auto now = std::chrono::system_clock::now();
        const auto today = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(now)};
        const std::string gameVersion = std::format("{}.{}.{}", static_cast<int>(today.year()), static_cast<unsigned>(today.month()), static_cast<unsigned>(today.day()));

        std::mt19937 gen(std::random_device{}());
        universe_id_t universeId = 0;
        do
        {
            std::uniform_int_distribution<universe_id_t> universeIdDist(1, std::numeric_limits<universe_id_t>::max());
            universeId = universeIdDist(gen);
        } while (universeIdToName.contains(universeId));

        statement.bind(1, universeId);
        statement.bind(2, universeName);
        statement.bind(3, seed);
        statement.bind(4, gameVersion);

        return statement.exec() > 0;
    }

    bool UniverseUtils::DeleteUniverse(const std::string& universeName)
    {
        std::string serverUniversePath{"data/server/universes/" + universeName};
        if (std::filesystem::exists(serverUniversePath))
            std::filesystem::remove_all(serverUniversePath);

        std::string clientUniversePath{"data/client/universes/" + universeName};
        if (std::filesystem::exists(clientUniversePath))
            std::filesystem::remove_all(clientUniversePath);

        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement statement(*db, "DELETE FROM universe WHERE universeName = ?");
        statement.bind(1, universeName);

        return statement.exec() > 0;
    }

    void UniverseUtils::AddUniverse(const std::string& universeName, universe_id_t universeId)
    {
        universeIdToName.emplace(universeId, universeName);
        universeNameToId.emplace(universeName, universeId);
    }

    void UniverseUtils::RemoveUniverse(universe_id_t universeId)
    {
        std::string universeName{};
        if (universeIdToName.contains(universeId))
        {
            universeName = universeIdToName[universeId];
            universeIdToName.erase(universeId);
        }
        if (universeNameToId.contains(universeName))
            universeNameToId.erase(universeName);
    }

    std::unordered_map<universe_id_t, std::string>& UniverseUtils::GetUniverses()
    {
        return universeIdToName;
    }

    universe_id_t UniverseUtils::GetUniverseId(const std::string& universeName)
    {
        if (universeNameToId.contains(universeName))
            return universeNameToId[universeName];

        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement query(*db, "SELECT universeId FROM universe WHERE universeName = ?");
        query.bind(1, universeName);

        if (query.executeStep())
        {
            return static_cast<universe_id_t>(query.getColumn(0).getInt());
        }

        return 0;
    }

    std::string UniverseUtils::GetUniverseName(universe_id_t universeId)
    {
        if (universeIdToName.contains(universeId))
            return universeIdToName[universeId];

        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement query(*db, "SELECT universeName FROM universe WHERE universeId = ?");
        query.bind(1, universeId);

        if (query.executeStep())
        {
            return query.getColumn(0).getString();
        }

        return "";
    }
    uint32_t UniverseUtils::GetUniverseSeed(universe_id_t universeId)
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement query(*db, "SELECT universeSeed FROM universe WHERE universeId = ?");
        query.bind(1, universeId);

        if (query.executeStep())
        {
            return static_cast<uint32_t>(query.getColumn(0).getInt());
        }

        return 0;
    }
}
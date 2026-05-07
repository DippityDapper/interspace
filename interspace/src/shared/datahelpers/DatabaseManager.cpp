#include "interspace/shared/datahelpers/DatabaseManager.hpp"

namespace Interspace
{
    void DatabaseManager::Init()
    {
        if (!std::filesystem::exists("data"))
            std::filesystem::create_directory("data");
        if (!std::filesystem::exists("data/shared"))
            std::filesystem::create_directory("data/shared");

        sharedDb = std::make_unique<SQLite::Database>("data/shared/shared.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        sharedDb->exec("PRAGMA foreign_keys = ON;");
    }

    SQLite::Database* DatabaseManager::GetSharedDatabase()
    {
        return sharedDb.get();
    }
}
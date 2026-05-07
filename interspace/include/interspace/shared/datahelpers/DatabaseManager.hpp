#pragma once
#include "SQLiteCpp/Database.h"

#include <memory>

namespace Interspace
{
    class DatabaseManager
    {
      private:
        static inline std::unique_ptr<SQLite::Database> sharedDb = nullptr;

      public:
        static void Init();

        static SQLite::Database* GetSharedDatabase();
    };
}
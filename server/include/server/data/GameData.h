#pragma once

#include "SQLiteCpp/SQLiteCpp.h"
#include "SDL3/SDL.h"

namespace Game
{
    class GameData
    {
    public:
        static SQLite::Database db;

    public:
        void Init();
        static std::string GetName(int cltNo);
    };
}

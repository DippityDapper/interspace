#pragma once

#include "SQLiteCpp/SQLiteCpp.h"
#include "SDL3/SDL.h"

#include "common/vector/Vec2.h"

namespace Game
{
    class GameData
    {
    public:
        static SQLite::Database db;

    public:
        void Init();

        static std::string GetUser(int userNo);
        static int GetUserNumber(std::string& userName);

        static void AddUser(int userNo, std::string& userName);
        static bool UserExists(std::string& userName);

        static void AddOrUpdateCharacter(int userNo, float x, float y);
        static bool GetCharacterPosition(int userNo, Engine::Vec2<float> &outPos);
    };
}

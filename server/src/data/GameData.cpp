#include "server/data/GameData.h"

namespace Game
{
    SQLite::Database GameData::db
    {
        "databases/test.sqlite",
        SQLite::OPEN_READWRITE
    };

    void GameData::Init()
    {

    }

    std::string GameData::GetUser(int userNo)
    {
        SQLite::Statement query(db,"select UserName from user where UserNo = ?");
        query.bind(1, userNo);

        if (query.executeStep())
        {
            return query.getColumn(0).getString();
        }
        else
        {
            return "";
        }
    }

    int GameData::GetUserNumber(std::string& userName)
    {
        SQLite::Statement query(db,"select UserNo from user where UserName = ?");
        query.bind(1, userName);

        if (query.executeStep())
        {
            SDL_Log("User found: %s", userName.c_str());
            return query.getColumn(0).getInt();
        }
        else
        {
            SDL_Log("User not found: %s", userName.c_str());
            return -1;
        }
    }

    void GameData::AddUser(int userNo, std::string& userName)
    {
        if (UserExists(userName))
            return;

        SQLite::Statement query(db,"insert into user(UserNo, UserName) values (?, ?);");
        query.bind(1, userNo);
        query.bind(2, userName);
        query.exec();
    }

    bool GameData::UserExists(std::string &userName)
    {
        SQLite::Statement query(db, "select 1 from user where UserName = ?");
        query.bind(1, userName);

        return query.executeStep();
    }

    void GameData::AddOrUpdateCharacter(int userNo, float x, float y)
    {
        // Try to update first
        SQLite::Statement update(db, "update character set LogoutPositionX = ?, LogoutPositionY = ? where UserNo = ?");
        update.bind(1, x);
        update.bind(2, y);
        update.bind(3, userNo);

        if (update.exec() == 0) // 0 rows updated, insert instead
        {
            SQLite::Statement insert(db, "insert into character(UserNo, LogoutPositionX, LogoutPositionY) VALUES (?, ?, ?)");
            insert.bind(1, userNo);
            insert.bind(2, x);
            insert.bind(3, y);
            insert.exec();
        }
    }

    bool GameData::GetCharacterPosition(int userNo, Engine::Vec2<float> &outPos)
    {
        SQLite::Statement query(db, "select LogoutPositionX, LogoutPositionY from character where UserNo = ?");
        query.bind(1, userNo);

        if (query.executeStep())
        {
            outPos.x = query.getColumn(0).getDouble();
            outPos.y = query.getColumn(1).getDouble();

            SDL_Log("User (%u) logged in with position (%f, %f)", userNo, outPos.x, outPos.y);

            return true;
        }
        SDL_Log("User (%u) logged in for the first time", userNo);
        return false;
    }
}

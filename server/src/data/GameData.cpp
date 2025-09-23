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

    std::string GameData::GetName(int cltNo)
    {
        SQLite::Statement query(db,"select username from user where userno = ?");
        query.bind(1, cltNo);

        if (query.executeStep())
        {
            return query.getColumn(0).getString();
        }
        else
        {
            return "";
        }
    }
}

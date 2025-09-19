#ifndef SDL3_FIRST_PROJECT_GAMEDATA_H
#define SDL3_FIRST_PROJECT_GAMEDATA_H

#include <SQLiteCpp/SQLiteCpp.h>
#include "SDL3/SDL.h"

class GameData
{
public:
    static SQLite::Database db;

public:
    void Init();
    static std::string GetName(int cltNo);
};


#endif

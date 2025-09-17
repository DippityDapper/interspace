#include "Area.h"

SDL_AppResult Area::Init()
{
    grid.InitializeGrid(25, 25, 64, 64);
    for (auto& kvp : grid.tiles)
    {
        if (kvp.first.x > 4 && kvp.first.x < 15 && kvp.first.y > 4 && kvp.first.y < 15)
            continue;
        Tile* tile = &kvp.second;
        tile->texture = nullptr;
    }

    return SDL_APP_SUCCESS;
}

void Area::Update(float delta)
{

}
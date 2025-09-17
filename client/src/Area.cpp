#include "Area.h"
#include "../../common/src/Packets.h"
#include "NetworkManager.h"
#include "Game.h"

SDL_AppResult Area::Init()
{
    SDL_Texture* tileTexture = ResourceLoader::LoadTexture(Game::state->renderer, "tiles/station_floor_tile_1.png");
    grid.InitializeGrid(25, 25, tileTexture->w, tileTexture->h);
    for (auto& kvp : grid.tiles)
    {
        if (kvp.first.x > 4 && kvp.first.x < 15 && kvp.first.y > 4 && kvp.first.y < 15)
            continue;
        Tile* tile = &kvp.second;
        tile->texture = tileTexture;
    }

    return SDL_APP_SUCCESS;
}

void Area::RenderTiles(SDL_Renderer *renderer, Camera& camera)
{
    for (auto& kvp : grid.tiles)
    {
        kvp.second.Render(renderer, camera);
    }
}

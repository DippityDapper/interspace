#include "client/world/Area.h"

namespace Game
{
    void Area::Init(SDL_Renderer* renderer)
    {
        std::string texturePath = "tiles/station_floor_tile_1.png";
        SDL_Texture* tileTexture = Engine::ResourceLoader::LoadTexture(renderer, texturePath);

        grid.InitializeGrid(25, 25, tileTexture->w, tileTexture->h);
        for (auto& kvp : grid.tiles)
        {
            if (kvp.first.x > 4 && kvp.first.x < 15 && kvp.first.y > 4 && kvp.first.y < 15)
                continue;
            Engine::Tile* tile = &kvp.second;
            tile->texture = tileTexture;
        }
    }

    void Area::RenderTiles(SDL_Renderer *renderer, Engine::Camera& camera)
    {
        for (auto& kvp : grid.tiles)
        {
            kvp.second.Render(renderer, camera);
        }
    }
}

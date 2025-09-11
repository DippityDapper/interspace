#ifndef SDL3_FIRST_PROJECT_TILE_H
#define SDL3_FIRST_PROJECT_TILE_H

#include "Vec2.h"
#include "SDL3/SDL.h"

struct Tile
{
    Vec2 gridPosition{};
    SDL_Texture* texture = nullptr;

    void Render(SDL_Renderer *renderer) const
    {
        SDL_FRect dest;

        dest.w = (float)texture->w;
        dest.h = (float)texture->h;
        dest.x = gridPosition.x * texture->w;
        dest.y = gridPosition.y * texture->h;

        SDL_RenderTexture(renderer, texture, nullptr, &dest);
    }
};


#endif

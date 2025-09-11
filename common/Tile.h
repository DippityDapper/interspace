#ifndef SDL3_FIRST_PROJECT_TILE_H
#define SDL3_FIRST_PROJECT_TILE_H

#include "Position.h"
#include "SDL3/SDL.h"

struct Tile
{
    Position gridPosition{};
    SDL_Texture* texture = nullptr;

    void Render(SDL_Renderer *renderer) const
    {
        SDL_FRect dest;

        dest.w = (float)texture->w;
        dest.h = (float)texture->h;
        dest.x = gridPosition.x * 64;
        dest.y = gridPosition.y * 64;

        SDL_RenderTexture(renderer, texture, nullptr, &dest);
    }
};


#endif

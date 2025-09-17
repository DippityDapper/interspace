#ifndef SDL3_FIRST_PROJECT_TILE_H
#define SDL3_FIRST_PROJECT_TILE_H

#include "../../common/src/Vec2.h"
#include "SDL3/SDL.h"
#include "Camera.h"

struct Tile
{
    Vec2 gridPosition{0,0};
    SDL_Texture* texture = nullptr;

    void Render(SDL_Renderer *renderer, Camera& camera) const
    {
        SDL_FRect dest;

        dest.w = (float)texture->w * camera.zoom;
        dest.h = (float)texture->h * camera.zoom;
        dest.x = (gridPosition.x * texture->w - camera.position.x) * camera.zoom;
        dest.y = (gridPosition.y * texture->h - camera.position.y) * camera.zoom;

        SDL_RenderTexture(renderer, texture, nullptr, &dest);
    }
};


#endif

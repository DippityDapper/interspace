#pragma once

#include "SDL3/SDL.h"

#include "common/vector/Vec2.h"

#include "client/camera/Camera.h"

namespace Engine
{
    struct Tile
    {
        Vec2<int> gridPosition{0,0};
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

}

#pragma once

#include <unordered_map>
#include <string>

#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"

namespace Engine
{
    class ResourceLoader
    {
    private:
        static std::unordered_map<std::string, SDL_Texture*> textures;

    public:
        static SDL_Texture* LoadTexture(SDL_Renderer *renderer, std::string& path);
        static void UnloadTexture(std::string& path);
        static void UnloadAll();
    };
}

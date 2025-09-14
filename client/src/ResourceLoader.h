#ifndef SDL3_FIRST_PROJECT_RESOURCELOADER_H
#define SDL3_FIRST_PROJECT_RESOURCELOADER_H

#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include <unordered_map>
#include <string>

class ResourceLoader
{
private:
    static std::unordered_map<std::string, SDL_Texture*> textures;
public:
    static SDL_Texture* LoadTexture(SDL_Renderer *renderer, const char* path);
    static void UnloadTexture(const char* path);
    static void UnloadAll();
};

#endif
#include "ResourceLoader.h"

std::unordered_map<std::string, SDL_Texture*> ResourceLoader::textures;

SDL_Texture* ResourceLoader::LoadTexture(SDL_Renderer *renderer, const char *path)
{
    std::string fullPath("assets/");
    fullPath += path;

    auto iterator = textures.find(fullPath);
    if (iterator != textures.end())
    {
        return iterator->second;
    }

    SDL_Texture *texture = IMG_LoadTexture(renderer, fullPath.c_str());
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    if (!texture)
    {
        SDL_Log("IMG_LoadTexture failed: %s", SDL_GetError());
    }
    if (texture)
    {
        textures[fullPath] = texture;
    }

    return texture;
}

void ResourceLoader::UnloadTexture(const char* path)
{
    auto iterator = textures.find(path);
    if (iterator != textures.end())
    {
        SDL_DestroyTexture(iterator->second);
        textures.erase(iterator);
    }
}

void ResourceLoader::UnloadAll()
{
    for (auto& pair : textures)
    {
        SDL_DestroyTexture(pair.second);
    }
    textures.clear();
}
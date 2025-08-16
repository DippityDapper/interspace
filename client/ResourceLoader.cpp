#include "ResourceLoader.h"

std::unordered_map<std::string, SDL_Texture*> ResourceLoader::textures;

SDL_Texture* ResourceLoader::LoadTexture(SDL_Renderer *renderer, const char *path)
{
    char *fullPath = nullptr;
    SDL_asprintf(&fullPath, "%s../assets/%s", SDL_GetBasePath(), path);

    std::string key(fullPath);

    auto iterator = textures.find(key);
    if (iterator != textures.end())
    {
        SDL_free(fullPath);
        return iterator->second;
    }

    SDL_Texture *texture = IMG_LoadTexture(renderer, fullPath);
    if (!texture)
    {
        SDL_Log("IMG_LoadTexture failed: %s", SDL_GetError());
    }
    if (texture)
    {
        textures[key] = texture;
    }

    SDL_free(fullPath);
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
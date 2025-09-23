#include "client/entity/ClientEntity.h"

namespace Game
{
    ClientEntity::ClientEntity(SDL_Renderer* renderer, std::string& path, float x, float y)
    {
        sprite.texture = Engine::ResourceLoader::LoadTexture(renderer, path);
        sprite.w = sprite.texture->w;
        sprite.h = sprite.texture->h;

        position.y = y;
        position.x = x;
    }

    void ClientEntity::Render(SDL_Renderer *renderer, Engine::Camera& camera) const
    {
        SDL_FRect dest;

        dest.w = (float)sprite.w * camera.zoom;
        dest.h = (float)sprite.h * camera.zoom;
        dest.x = (position.x - (float)sprite.w/2.0f - camera.position.x) * camera.zoom;
        dest.y = (position.y - (float)sprite.h/2.0f - camera.position.y) * camera.zoom;

        SDL_RenderTexture(renderer, sprite.texture, nullptr, &dest);
    }

    void ClientEntity::SetPosition(float x, float y)
    {
        position.x = x;
        position.y = y;
    }
}

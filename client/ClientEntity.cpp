#include "ClientEntity.h"

ClientEntity::ClientEntity(SDL_Renderer* renderer, const char* path, float x, float y)
{
    sprite.texture = ResourceLoader::LoadTexture(renderer, path);
    sprite.w = sprite.texture->w;
    sprite.h = sprite.texture->h;

    position.y = y;
    position.x = x;
}

void ClientEntity::Update(float delta)
{
    Position dir{0, 0};

    if (inputs[UP])
    {
        dir.y -= 1;
    }
    if (inputs[DOWN])
    {
        dir.y += 1;
    }
    if (inputs[LEFT])
    {
        dir.x -= 1;
    }
    if (inputs[RIGHT])
    {
        dir.x += 1;
    }

    position += dir.Normalized() * speed * delta;
}

void ClientEntity::Render(SDL_Renderer *renderer)
{
    SDL_FRect dest;

    dest.w = sprite.w;
    dest.h = sprite.h;
    dest.x = position.x + sprite.w/2.0f;
    dest.y = position.y + sprite.h/2.0f;

    SDL_RenderTexture(renderer, sprite.texture, nullptr, &dest);
}

void ClientEntity::SetPosition(float x, float y)
{
    position.x = x;
    position.y = y;
}

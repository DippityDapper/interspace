#ifndef SDL3_FIRST_PROJECT_CLIENTENTITY_H
#define SDL3_FIRST_PROJECT_CLIENTENTITY_H

#include "SDL3/SDL.h"
#include "ResourceLoader.h"
#include "../../common/src/Vec2.h"

struct Sprite
{
    SDL_Texture *texture = nullptr;
    int w = 0;
    int h = 0;
};

class ClientEntity
{
private:
    Sprite sprite{};
    Vec2 position{0,0};
public:
    ClientEntity(SDL_Renderer* renderer, const char* path, float x, float y);
    void Render(SDL_Renderer* renderer) const;
    void SetPosition(float x, float y);
};

#endif

#ifndef SDL3_FIRST_PROJECT_CLIENTENTITY_H
#define SDL3_FIRST_PROJECT_CLIENTENTITY_H

#include "SDL3/SDL.h"
#include "ResourceLoader.h"
#include "../common/Position.h"

struct Sprite
{
    SDL_Texture *texture = nullptr;
    int w = 0;
    int h = 0;
};

enum Directions
{
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};

class ClientEntity
{
private:
    Sprite sprite;
    Position position;
    bool inputs[4];

    float speed = 200;
public:
    ClientEntity(SDL_Renderer* renderer, const char* path, float x, float y);
    void Update(float delta);
    void Render(SDL_Renderer* renderer);
    void SetPosition(float x, float y);
};

#endif

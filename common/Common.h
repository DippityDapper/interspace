#ifndef SDL3_FIRST_PROJECT_COMMON_H
#define SDL3_FIRST_PROJECT_COMMON_H

#include "SDL3/SDL.h"
#include "enet/enet.h"

struct AppState
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    uint64_t lastTick = 0;
    uint64_t currentTick = 0;
    float deltaTime = 0;
};

#endif

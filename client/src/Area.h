#ifndef SDL3_FIRST_PROJECT_AREA_H
#define SDL3_FIRST_PROJECT_AREA_H


#include <map>
#include <memory>
#include "ClientEntity.h"
#include "../../common/src/Grid.h"
#include "enet/enet.h"

class Area
{
public:
    Grid grid{};

public:
    SDL_AppResult Init();

public:
    void RenderTiles(SDL_Renderer *renderer, Camera& camera);
};


#endif

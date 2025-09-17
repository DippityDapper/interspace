#ifndef SDL3_FIRST_PROJECT_AREA_H
#define SDL3_FIRST_PROJECT_AREA_H


#include <map>
#include <memory>
#include "../../common/src/Grid.h"
#include "enet/enet.h"

class Area
{
public:
    Grid grid{};

public:
    SDL_AppResult Init();
    void Update(float delta);
};


#endif

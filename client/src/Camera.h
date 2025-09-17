#ifndef SDL3_FIRST_PROJECT_CAMERA_H
#define SDL3_FIRST_PROJECT_CAMERA_H

#include "SDL3/SDL.h"
#include "../../common/src/Vec2.h"

class Camera
{
public:
    Vec2 position{0,0};
    float zoom = 1.0f;
    bool panning = false;

public:
    void Update(float delta);
    void HandleEvents(SDL_Event& event);
};


#endif

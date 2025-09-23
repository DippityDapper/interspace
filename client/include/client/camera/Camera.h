#pragma once

#include "SDL3/SDL.h"

#include "common/vector/Vec2.h"

namespace Engine
{
    class Camera
    {
    public:
        Engine::Vec2<float> position{0,0};
        float zoom = 1.0f;
        bool panning = false;

    public:
        void Update(float delta);
        void HandleEvents(SDL_Event& event);
    };
}

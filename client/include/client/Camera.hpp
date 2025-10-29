#pragma once

#include "dapper2d/Camera.hpp"

namespace Game
{
class Camera : public Engine::Camera
    {
    private:
        Engine::Vec2<float> targetPosition{0,0};
        float targetZoom = 1.0;

public:
    float panSpeed = 10;
    float zoomSpeed = 10;

    public:
        Camera();
        Camera(float x, float y, float zoom);

        void Update(float delta) override;
        void HandleEvents(SDL_Event& event) override;
    };
}

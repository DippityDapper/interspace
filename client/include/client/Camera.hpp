#pragma once

#include <map>

#include "dapper2d/Camera.hpp"

namespace Game
{
class Camera : public Engine::Camera
    {
    private:
        Engine::Vec2<float> targetPosition{0,0};
        float targetZoom = 1.0;

        std::map<unsigned int, bool> heldKeys;

public:
    float panSpeed = 10;
    float zoomSpeed = 10;
    float moveSpeed = 2; // In tiles

    public:
        Camera();
        Camera(float x, float y, float zoom);

        void Update(float delta) override;
        void HandleEvents(SDL_Event& event) override;
    };
}

#pragma once

#include "igneous/engine/Camera.hpp"

namespace Interspace
{
    class Camera : public Engine::Camera
    {
    private:
        Engine::Vec2<int> oldViewport{};

    public:
        Engine::Vec2<float> targetPosition{0,0};
        float targetZoom = 1.0;

        Engine::Vec2<float> velocity{0,0};

        float panSpeed = 10;
        float zoomSpeed = 10;
        float moveSpeed = 20;
        float moveSpeedMultiplier = 1.0;

    public:
        Camera();
        Camera(float x, float y, float zoom);

        void Update(float delta) override;
        void HandleEvents(Engine::InputLayer& layer) override;
    };
}

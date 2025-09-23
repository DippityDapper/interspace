#pragma once

#include <vector>

#include "SDL3/SDL.h"

#include "common/vector/Vec2.h"

namespace Game
{
    class ServerEntity
    {
    private:
        Engine::Vec2<float> position{0,0};

        float speed = 300;
        std::vector<Engine::Vec2<int>> path{};
        Engine::Vec2<float> target{0,0};
        bool isPathing = false;

    public:
        ServerEntity(float x, float y);
        void Update(float delta);
        void SetPosition(float x, float y);
        Engine::Vec2<float> GetPosition();
        void SetPath(const std::vector<Engine::Vec2<int>>& newPath);
    };
}

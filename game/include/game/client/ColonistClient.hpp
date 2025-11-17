#pragma once

#include <string>

#include "dapper2d/Sprite.hpp"
#include "dapper2d/Vec2.hpp"

namespace Game
{
    class ColonistClient
    {
    public:
        Engine::Vec2<float> position{};
        uint32_t id = 0;
        std::string name{};

        std::unique_ptr<Engine::Sprite> sprite = nullptr;

    public:
        ColonistClient() = default;
        ColonistClient(uint32_t _id, const std::string& _name, const Engine::Vec2<float>& _position);
        void Render();
    };
}

#pragma once

#include <string>

#include "dapper2d/Vec2.hpp"

namespace Engine
{
    class Sprite;
}

namespace Game
{
    class ClientEntity
    {
    private:
        Engine::Sprite* sprite = nullptr;
        Engine::Vec2<float> position{0,0};

    public:
        std::string username;

    public:
        ClientEntity(std::string& texturePath, std::string& _username, float x, float y);
        ~ClientEntity();

        void Render() const;
        void SetPosition(float x, float y);
    };
}

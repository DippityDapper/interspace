#pragma once

#include "SDL3/SDL.h"
#include "imgui.h"

#include "dapper2d/Vec2.hpp"
#include "dapper2d/ResourceLoader.hpp"
#include "dapper2d/Camera.hpp"
#include "dapper2d/Sprite.hpp"
#include "dapper2d/Renderer.hpp"

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

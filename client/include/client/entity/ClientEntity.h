#pragma once

#include "SDL3/SDL.h"
#include "imgui.h"

#include "common/vector/Vec2.h"

#include "client/resources/ResourceLoader.h"
#include "client/camera/Camera.h"
#include "client/entity/Sprite.h"
#include "client/engine/Renderer.h"

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

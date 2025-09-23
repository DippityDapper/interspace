#pragma once

#include "SDL3/SDL.h"
#include "imgui.h"

#include "common/vector/Vec2.h"

#include "client/resources/ResourceLoader.h"
#include "client/camera/Camera.h"

namespace Game
{
    struct Sprite
    {
        SDL_Texture *texture = nullptr;
        int w = 0;
        int h = 0;
    };

    class ClientEntity
    {
    private:
        Sprite sprite{};
        Engine::Vec2<float> position{0,0};

    public:
        std::string username;

    public:
        ClientEntity(SDL_Renderer* renderer, std::string& path, std::string& _username, float x, float y);

        void Render(SDL_Renderer* renderer, Engine::Camera& camera) const;
        void SetPosition(float x, float y);
    };
}

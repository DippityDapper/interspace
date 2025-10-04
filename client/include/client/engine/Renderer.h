#pragma once

#include "SDL3/SDL.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

#include "common/vector/Vec2.h"

#include "client/engine/Window.h"
#include "client/camera/Camera.h"
#include "client/entity/Sprite.h"

namespace Engine
{
    class Renderer
    {
    private:
        static SDL_Renderer* renderer;

    public:
        void Init();
        void Render();
        void Clean();

        static void BufferClear();
        static void BufferAdd(Vec2<float> position, Sprite* sprite);
        static SDL_Renderer* GetRenderer();
    };
}

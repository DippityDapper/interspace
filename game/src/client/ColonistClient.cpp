#include "game/client/ColonistClient.hpp"

#include <algorithm>

#include "imgui.h"

#include "dapper2d/Camera.hpp"
#include "dapper2d/Renderer.hpp"
#include "dapper2d/Window.hpp"

namespace Game
{
    ColonistClient::ColonistClient(uint32_t _id, const std::string& _name, const Engine::Vec2<float>& _position)
    {
        id = _id;
        name = _name;
        position = _position;
        sprite = std::make_unique<Engine::Sprite>("assets/colonists/colonist_blue_spritesheet.png", 16, 16, 0, 0);
        sprite->scaleX = 2.0f;
        sprite->scaleY = 2.0f;
    }

    void ColonistClient::Render()
    {
        Engine::Renderer::BufferAdd(position, sprite.get(), nullptr);

        Engine::Vec2<int> viewport = Engine::Window::viewport;
        Engine::Camera* camera = Engine::Camera::main;

        float camPosX = camera->position.x;
        float camPosY = camera->position.y;

        float screenX = (position.x - camPosX) * camera->zoom + viewport.x * 0.5f;
        float screenY = (position.y - camPosY) * camera->zoom + viewport.y * 0.5f;

        float yOffset = ((sprite->atlasH * sprite->scaleY * 0.75f) / 2.0f) * camera->zoom;

        ImGui::SetNextWindowPos({screenX, screenY - yOffset}, 0, {0.5f, 1.0f});

        ImGui::Begin(
            name.c_str(), nullptr,
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize
        );

        float scale = camera->zoom * 0.7f;
        ImGui::SetWindowFontScale(scale);

        ImGui::Text("%s", name.c_str());

        ImGui::End();
    }
}

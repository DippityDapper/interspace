#include "interspace/client/Colonist.hpp"

#include "imgui.h"
#include "igneous/engine/Camera.hpp"
#include "igneous/rendering/Window.hpp"

namespace Interspace::Client
{
    Colonist::Colonist()
    {
        sprite = std::make_unique<Engine::Sprite>(entityData.position, "assets/colonists/colonist_blue_spritesheet.png", 16, 16, 0, 0);
        sprite->scaleX = 2;
        sprite->scaleY = 2;
    }

    void Colonist::Update(float delta)
    {
    }

    void Colonist::RenderName(const std::string& factionName)
    {
        Engine::Vec2<int> viewport = Engine::Window::viewport;
        Engine::Camera* camera = Engine::Camera::main;

        float scale = camera->zoom * 0.7f;
        if (scale < camera->minZoom / 0.25f)
            return;

        float camPosX = camera->position.x;
        float camPosY = camera->position.y;

        float screenX = (entityData.position.x - camPosX) * camera->zoom + viewport.x * 0.5f;
        float screenY = (entityData.position.y - camPosY) * camera->zoom + viewport.y * 0.5f;

        float yOffset = ((sprite->atlasH * sprite->scaleY * 0.75f) / 2.0f) * camera->zoom;

        ImGui::SetNextWindowPos({screenX, screenY - yOffset}, 0, {0.5f, 1.0f});

        std::string idStr = entityData.name + std::to_string(entityData.id);
        ImGui::Begin(
            idStr.c_str(), nullptr,
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize
        );

        ImGui::SetWindowFontScale(scale);

        std::string fullName = entityData.name + " [" + factionName + "]";
        ImGui::Text("%s", fullName.c_str());

        ImGui::End();
    }
}

#include "client/ClientEntity.hpp"

namespace Game
{
    ClientEntity::ClientEntity(std::string& texturePath, std::string& _username, float x, float y)
    {
        username = _username;
        sprite = new Engine::Sprite(texturePath);

        position.y = y;
        position.x = x;
    }

    ClientEntity::~ClientEntity()
    {
        delete sprite;
    }

    void ClientEntity::Render() const
    {
        Engine::Camera* mainCamera = Engine::Camera::main;
        if (mainCamera == nullptr)
            return;

        Engine::Renderer::BufferAdd(position, sprite);

        SDL_FRect dest;

        dest.w = (float)sprite->w * mainCamera->zoom;
        dest.h = (float)sprite->h * mainCamera->zoom;
        dest.x = (position.x - mainCamera->position.x) * mainCamera->zoom;
        dest.y = (position.y - mainCamera->position.y) * mainCamera->zoom;

        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::SetNextWindowPos(ImVec2(dest.x + dest.w/2.0f, dest.y + dest.h + 2), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
        ImGui::Begin(("##username_" + username).c_str(), nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoInputs);

        ImGui::Text("%s", username.c_str());
        ImGui::End();
    }

    void ClientEntity::SetPosition(float x, float y)
    {
        position.x = x - (float)sprite->w/2.0f;
        position.y = y - (float)sprite->h/2.0f;
    }
}

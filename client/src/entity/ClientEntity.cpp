#include "client/entity/ClientEntity.h"

namespace Game
{
    ClientEntity::ClientEntity(SDL_Renderer* renderer, std::string& path, std::string& _username, float x, float y)
    {
        username = _username;

        sprite.texture = Engine::ResourceLoader::LoadTexture(renderer, path);
        sprite.w = sprite.texture->w;
        sprite.h = sprite.texture->h;

        position.y = y;
        position.x = x;
    }

    void ClientEntity::Render(SDL_Renderer *renderer, Engine::Camera& camera) const
    {
        SDL_FRect dest;

        dest.w = (float)sprite.w * camera.zoom;
        dest.h = (float)sprite.h * camera.zoom;
        dest.x = (position.x - (float)sprite.w/2.0f - camera.position.x) * camera.zoom;
        dest.y = (position.y - (float)sprite.h/2.0f - camera.position.y) * camera.zoom;

        SDL_RenderTexture(renderer, sprite.texture, nullptr, &dest);

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
        position.x = x;
        position.y = y;
    }
}

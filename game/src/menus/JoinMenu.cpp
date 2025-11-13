#include "game/menus/JoinMenu.hpp"

#include "imgui.h"

#include "dapper2d/Scenes.hpp"
#include "dapper2d/Window.hpp"

#include "game/game/Game.hpp"
#include "SDL3/SDL_log.h"

namespace Game
{
    void JoinMenu::Init()
    {
    }

    void JoinMenu::Render()
    {
        Engine::Vec2<int> viewport = Engine::Window::viewport;

        const float windowWidth = Engine::Window::viewport.x/2.0f;
        const float windowHeight = Engine::Window::viewport.y/2.0f;

        ImGui::SetNextWindowPos({windowWidth, windowHeight}, 0, {0.5f,0.5f});

        ImGui::Begin("Join World", nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_AlwaysAutoResize
        );

        ImGui::InputText("Username", usernameLineEdit, sizeof(usernameLineEdit));
        ImGui::InputText("ip Address", ipLineEdit, sizeof(ipLineEdit));
        // ImGui::InputText("Port", portLineEdit, sizeof(portLineEdit));

        if (ImGui::Button("Back"))
        {
            Engine::Scenes::LoadScene("multiplayer_menu");
        }
        ImGui::SameLine();

        const float joinButtonWidth = 80.0f;
        const float joinButtonHeight = 32.0f;
        ImVec2 joinButtonSize(joinButtonWidth, joinButtonHeight);

        float availableWidth = ImGui::GetContentRegionAvail().x;
        float cursorPosX = ImGui::GetCursorPosX() + availableWidth - joinButtonWidth;
        ImGui::SetCursorPosX(cursorPosX);

        if (ImGui::Button("Join", joinButtonSize))
        {
            if (JoinWorld())
                Engine::Scenes::RemoveAllScenes();
        }

        ImGui::End();
    }

    bool JoinMenu::JoinWorld()
    {
        std::string ipStr = ipLineEdit;
        if (ipStr.empty())
            return false;

        // std::string portStr = portLineEdit;
        // if (portStr.empty())
        //     return false;

        std::string usernameStr = usernameLineEdit;
        if (usernameStr.empty())
            return false;

        // int port = -1;
        // try
        // {
        //     port = std::stoi(portStr);
        // }
        // catch (std::exception& e)
        // {
        //     SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", e.what());
        //     return false;
        // }

        if (!Game::JoinWorld(usernameStr, ipStr, 33333))
            return false;

        return true;
    }
}

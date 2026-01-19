#include "interspace/menus/MultiplayerMenu.hpp"

#include "imgui.h"

#include "igneous/scenes/SceneManager.hpp"
#include "igneous/rendering/Window.hpp"
#include "interspace/game/Sounds.hpp"

namespace Interspace
{
    void MultiplayerMenu::Init()
    {
    }

    void MultiplayerMenu::Render()
    {
        Engine::Vec2<int> viewport = Engine::Window::viewport;

        const float windowWidth = 128;
        const float windowHeight = 256;

        ImGui::SetNextWindowSize({windowWidth, windowHeight});
        ImGui::SetNextWindowPos({viewport.x * 0.5f, viewport.y * 0.5f}, 0, {0.5f, 0.5f});

        ImGui::Begin(
                "Multiplayer", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

        const float buttonWidth = 80.0f;
        const float buttonHeight = 32.0f;
        ImVec2 buttonSize(buttonWidth, buttonHeight);

        float availableWidth = ImGui::GetContentRegionAvail().x / 2.0f;
        float cursorPosX = ImGui::GetCursorPosX() + availableWidth - (buttonWidth / 2.0f);
        ImGui::SetCursorPosX(cursorPosX);

        if (ImGui::Button("Host", buttonSize))
        {
            Sounds::PlaySound("button_1", 1.0f);
            root->LoadScene("worlds_menu_multiplayer");
        }

        availableWidth = ImGui::GetContentRegionAvail().x / 2.0f;
        cursorPosX = ImGui::GetCursorPosX() + availableWidth - (buttonWidth / 2.0f);
        ImGui::SetCursorPosX(cursorPosX);

        if (ImGui::Button("Join", buttonSize))
        {
            Sounds::PlaySound("button_1", 1.0f);
            root->LoadScene("join_menu");
        }

        const float backButtonWidth = 64.0f;
        const float backButtonHeight = 32.0f;
        ImVec2 backButtonSize(backButtonWidth, backButtonHeight);

        availableWidth = ImGui::GetContentRegionAvail().x / 2.0f;
        cursorPosX = ImGui::GetCursorPosX() + availableWidth - (backButtonWidth / 2.0f);
        ImGui::SetCursorPosX(cursorPosX);

        float availableHeight = ImGui::GetContentRegionAvail().y;
        float cursorPosY = ImGui::GetCursorPosY() + availableHeight - backButtonHeight;
        ImGui::SetCursorPosY(cursorPosY);

        if (ImGui::Button("Back", backButtonSize))
        {
            Sounds::PlaySound("button_back", 1.0f);
            root->LoadScene("main_menu");
        }

        ImGui::End();
    }
}

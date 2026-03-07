#include "interspace/menus/JoinMenu.hpp"

#include "imgui.h"
#include "igneous/engine/CFGParser.hpp"

#include "igneous/scenes/SceneManager.hpp"
#include "igneous/rendering/Window.hpp"

#include "interspace/game/Game.hpp"
#include "interspace/game/Sounds.hpp"

namespace Interspace
{
    void JoinMenu::Init()
    {
    }

    void JoinMenu::Render()
    {
        Engine::Vec2<int> viewport = Engine::Window::viewport;

        const float windowWidth = viewport.x / 2.0f;
        const float windowHeight = viewport.y / 2.0f;

        ImGui::SetNextWindowPos({windowWidth, windowHeight}, 0, {0.5f, 0.5f});

        ImGui::Begin("Join World", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::InputText("ip Address", ipLineEdit, sizeof(ipLineEdit));

        if (ImGui::Button("Back"))
        {
            Sounds::PlaySound("button_back", 1.0f);
            root->LoadScene("multiplayer_menu");
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
            Sounds::PlaySound("button_1", 1.0f);
            JoinWorld();
        }

        ImGui::End();
    }

    bool JoinMenu::JoinWorld()
    {
        std::string ipStr = ipLineEdit;
        if (ipStr.empty())
            return false;

        Engine::CFGParser::LoadConfig("game_config.cfg", "game_config");
        std::string username = Engine::CFGParser::GetString("game_config", "username");
        if (username.empty())
            return false;

        if (!Game::JoinWorld(username, ipStr, 33333))
            return false;

        return true;
    }
}

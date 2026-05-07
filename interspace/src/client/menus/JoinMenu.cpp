#include "interspace/client/menus/JoinMenu.hpp"

#include "imgui.h"
#include "igneous/engine/CFGParser.hpp"

#include "igneous/scenes/SceneManager.hpp"
#include "igneous/rendering/Window.hpp"

#include "interspace/shared/game/Game.hpp"
#include "interspace/client/sounds/SoundManager.hpp"

namespace Interspace
{
    void JoinMenu::OnCreated()
    {
    }

    void JoinMenu::HandleInputs(Engine::InputLayer& layer)
    {
        if (!layer.Is("ui"))
            return;

        Engine::Vec2<int> viewport = Engine::Window::viewport;

        const float windowWidth = viewport.x / 2.0f;
        const float windowHeight = viewport.y / 2.0f;

        ImGui::SetNextWindowPos({windowWidth, windowHeight}, 0, {0.5f, 0.5f});

        ImGui::Begin("Join World", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::InputText("ip Address", ipLineEdit, sizeof(ipLineEdit));

        if (ImGui::Button("Back"))
        {
            SoundManager::PlaySound("button_back", 1.0f);
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
            SoundManager::PlaySound("button_1", 1.0f);
            JoinWorld();
        }

        ImGui::End();
    }

    bool JoinMenu::JoinWorld()
    {
        std::string ipStr = ipLineEdit;
        if (ipStr.empty())
            return false;

        if (!Game::JoinUniverse(ipStr))
            return false;

        return true;
    }
}

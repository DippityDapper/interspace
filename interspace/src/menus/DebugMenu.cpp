#include "interspace/menus/DebugMenu.hpp"

#include "imgui.h"
#include "interspace/game/Game.hpp"

namespace Interspace
{
    void DebugMenu::Init()
    {
    }

    void DebugMenu::Render()
    {
        ImGui::Begin("debug menu");

        if (ImGui::Button("Spawn Colonist"))
        {
            
        }

        ImGui::End();
    }
}

#include "interspace/menus/DebugMenu.hpp"

#include "igneous/input/InputLayer.hpp"
#include "imgui.h"

namespace Interspace
{
    void DebugMenu::Init()
    {
    }


    void DebugMenu::UI(Engine::InputLayer& layer)
    {
        ImGui::Begin("debug menu");

        if (ImGui::Button("Spawn Colonist"))
        {
            
        }

        ImGui::End();
    }
}

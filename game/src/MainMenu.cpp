#include "game/MainMenu.hpp"

#include "imgui.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"

#include "dapper2d/Scenes.hpp"
#include "dapper2d/Window.hpp"

#include "game/WorldCreationScene.hpp"

namespace Game
{
    void MainMenu::Init()
    {
    }

    void MainMenu::Update(float delta)
    {

    }

    void MainMenu::Render()
    {
        ImGui::SetNextWindowPos({Engine::Window::viewport.x/2.0f,Engine::Window::viewport.y/2.0f}, 0, {0.5f,0.5f});

        ImGui::Begin
        ("Main Menu", nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove
        );

        if (ImGui::Button("World Creation"))
        {
            if (!Engine::Scenes::SceneExists("world_creation_menu"))
            {
                WorldCreationScene* worldCreationScene = new WorldCreationScene();
                Engine::Scenes::CreateScene(worldCreationScene, "world_creation_menu");
            }
            Engine::Scenes::LoadScene("world_creation_menu");
        }

        ImGui::End();
    }

    void MainMenu::HandleEvents(SDL_Event& event)
    {
    }

    void MainMenu::Clean()
    {

    }
}

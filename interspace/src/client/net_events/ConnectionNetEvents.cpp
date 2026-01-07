#include "igneous/Scenes.hpp"
#include "interspace/client/World.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/menus/MainMenu.hpp"

namespace Interspace::Client
{
    void World::OnDisconnectAcknowledged(const std::vector<uint8_t>& data)
    {
        Game::Disconnect();

        if (!Engine::Scenes::SceneExists("main_menu"))
            Engine::Scenes::CreateScene(new MainMenu(), "main_menu");

        Engine::Scenes::LoadScene("main_menu");
    }
}

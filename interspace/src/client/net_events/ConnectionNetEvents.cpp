#include "igneous/scenes/SceneManager.hpp"
#include "interspace/client/World.hpp"
#include "interspace/game/Game.hpp"

namespace Interspace::Client
{
    void World::OnDisconnectAcknowledged(const std::vector<uint8_t>& data)
    {
        Game::Disconnect();

        if (Engine::SceneManager::GetSceneRoot()->SceneExists("main_menu"))
            Engine::SceneManager::GetSceneRoot()->LoadScene("main_menu");
    }
}

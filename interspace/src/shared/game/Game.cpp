#include "interspace/shared/game/Game.hpp"

#include "igneous/engine/PerlinNoise.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/scenes/SceneRoot.hpp"
#include "interspace/client/menus/MainMenu.hpp"

#include "interspace/client/sounds/SoundManager.hpp"
#include "interspace/shared/network/NetworkManager.hpp"
#include "interspace/shared/network/SteamManager.hpp"

namespace Interspace
{
    void Game::OnCreated()
    {
        singleton = true;

        SoundManager::Init();

        SteamManager::TryConnect();

        Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_PIXELART);
        Engine::Input::AddInputLayer("gameplay", 0);
        Engine::Input::AddInputLayer("ui", 1);

        root->AddScene<SteamManager>("steam_manager", "managers", true, true);
        root->AddScene<NetworkManager>("network_manager", "managers", true, true);

        root->AddScene<MainMenu>("main_menu", "main_menus", true);

    }
}

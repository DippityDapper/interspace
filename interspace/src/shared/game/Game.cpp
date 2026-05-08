#include "interspace/shared/game/Game.hpp"

#include "igneous/input/Input.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/scenes/SceneRoot.hpp"
#include "interspace/client/menus/MainMenu.hpp"

#include "interspace/client/sounds/SoundManager.hpp"
#include "interspace/shared/datahelpers/ClientManager.hpp"
#include "interspace/shared/datahelpers/DatabaseManager.hpp"
#include "interspace/shared/datahelpers/TileManager.hpp"
#include "interspace/shared/datahelpers/UniverseUtils.hpp"
#include "interspace/shared/datahelpers/WorldManager.hpp"
#include "interspace/shared/network/NetworkManager.hpp"
#include "interspace/shared/network/SteamManager.hpp"

namespace Interspace
{
    void Game::OnCreated()
    {
        singleton = true;

        SoundManager::Init();
        DatabaseManager::Init();
        UniverseUtils::Init();
        WorldManager::Init();
        ClientManager::Init();
        TileManager::Init();

        SteamManager::TryConnect();

        Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_PIXELART);
        Engine::Input::AddInputLayer("gameplay", 0);
        Engine::Input::AddInputLayer("ui", 1);

        root->AddScene<SteamManager>("steam_manager", "", true, true);
        root->AddScene<NetworkManager>("network_manager", "", true, true);

        root->AddScene<MainMenu>("main_menu", "main_menus", true);
    }
}

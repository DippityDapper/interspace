#include "interspace/shared/game/Game.hpp"

#include "igneous/input/Input.hpp"
#include "igneous/networking/LocalNetwork.hpp"
#include "igneous/networking/RemoteNetwork.hpp"
#include "igneous/resources/ResourceManager.hpp"

#include "igneous/scenes/SceneManager.hpp"
#include "igneous/scenes/SceneRoot.hpp"
#include "interspace/client/sounds/SoundManager.hpp"

#include "interspace/client/menus/MainMenu.hpp"
#include "interspace/shared/datahelpers/ClientManager.hpp"
#include "interspace/shared/datahelpers/DatabaseManager.hpp"
#include "interspace/shared/datahelpers/TileManager.hpp"
#include "interspace/shared/datahelpers/UniverseManager.hpp"
#include "interspace/shared/datahelpers/WorldManager.hpp"
#include "interspace/shared/network/NetworkManager.hpp"
#include "interspace/shared/network/SteamManager.hpp"

namespace Interspace
{
    void Game::OnCreated()
    {
        singleton = true;

        InitSystems();
        Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_PIXELART);
        Engine::Input::AddInputLayer("gameplay", 0);
        Engine::Input::AddInputLayer("ui", 1);

        if (!mainMenu)
            mainMenu = root->AddScene<MainMenu>("main_menu", true);
    }

    void Game::InitSystems()
    {
        SteamManager::Init();
        SteamManager::TryConnect();
        SoundManager::Init();
        DatabaseManager::Init();
        UniverseManager::Init();
        WorldManager::Init();
        ClientManager::Init();
        TileManager::Init();
    }

    void Game::Update(double delta)
    {
        SteamManager::RunCallbacks();
    }

    bool Game::HostUniverse(const std::string& universeName, int peerCount, bool localOnly)
    {
        NetworkManager::CreateRemoteClientServer(peerCount, localOnly);
        Engine::SceneManager::GetSceneRoot()->UnloadAllScenes();

        if (!serverUniverse)
        {
            serverUniverse = Engine::SceneManager::GetSceneRoot()->AddScene<Server::ServerUniverse>("server_universe", true, false);
        }

        WorldManager::LoadWorlds(universeName);
        serverUniverse->InitUniverse(NetworkManager::server.get());

        return true;
    }

    bool Game::JoinUniverse(const std::string& ip)
    {
        NetworkManager::CreateClient(ip);
        Engine::SceneManager::GetSceneRoot()->UnloadAllScenes();

        if (!clientUniverse)
        {
            clientUniverse = Engine::SceneManager::GetSceneRoot()->AddScene<Client::ClientUniverse>("client_universe", true, false);
        }

        clientUniverse->InitUniverse(NetworkManager::client.get());
        return true;
    }

    bool Game::LoadUniverse(const std::string& universeName)
    {
        NetworkManager::CreateLocalClientServer();
        Engine::SceneManager::GetSceneRoot()->UnloadAllScenes();

        if (!serverUniverse)
        {
            serverUniverse = Engine::SceneManager::GetSceneRoot()->AddScene<Server::ServerUniverse>("server_universe", true, false);
        }

        if (!clientUniverse)
        {
            clientUniverse = Engine::SceneManager::GetSceneRoot()->AddScene<Client::ClientUniverse>("client_universe", true, false);
        }

        WorldManager::LoadWorlds(universeName);
        serverUniverse->InitUniverse(NetworkManager::server.get());
        clientUniverse->InitUniverse(NetworkManager::client.get());

        return true;
    }
}

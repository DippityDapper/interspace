#include "interspace/game/Game.hpp"

#include "igneous/resources/ResourceManager.hpp"
#include "igneous/networking/NetworkManager.hpp"

#include "igneous/scenes/SceneManager.hpp"
#include "igneous/scenes/SceneRoot.hpp"
#include "interspace/game/Sounds.hpp"

#include "interspace/menus/MainMenu.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/server/World.hpp"
#include "interspace/world/TileRegistry.hpp"

namespace Interspace
{
    void Game::Init()
    {
        singleton = true;
        DBHelper::Init();
        TileRegistry::Init();
        Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_PIXELART);

        Sounds::AddSound("button_1", "assets/sounds/buttons/button_1.mp3", 0);
        Sounds::AddSound("button_back", "assets/sounds/buttons/button_back.mp3", 0);

        if (!mainMenu)
            mainMenu = root->AddScene<MainMenu>("main_menu", true);
    }

    void Game::Update(float delta)
    {
        if (pendingDisconnect)
        {
            pendingDisconnect = false;
            root->RemoveScene("client_world");
            clientWorld = nullptr;
            root->RemoveScene("server_world");
            serverWorld = nullptr;
            client = nullptr;
            server = nullptr;
            clientNetInterface = nullptr;
            serverNetInterface = nullptr;
        }

        if (serverNetInterface)
            serverNetInterface->Poll();
        if (clientNetInterface)
            clientNetInterface->Poll();
    }

    void Game::UI()
    {
    }

    void Game::HandleEvents(Engine::InputLayer& layer)
    {
    }

    void Game::Clean()
    {
    }

    bool Game::HostWorld(const std::string& worldName, int port, int peerCount, bool localOnly)
    {
        serverNetInterface = Engine::NetworkManager::CreateServer(port, peerCount, localOnly);
        server = std::make_unique<Server::Server>(serverNetInterface.get());

        if (!server->netInterface->Connected())
            return false;

        Engine::SceneManager::GetSceneRoot()->UnloadAllScenes();

        if (!serverWorld)
        {
            serverWorld = Engine::SceneManager::GetSceneRoot()->AddScene<Server::World>("server_world", true, false);
        }

        serverWorld->InitWorld(server.get(), worldName);
        return true;
    }

    bool Game::JoinWorld(const std::string& username, const std::string& ip, int port)
    {
        clientNetInterface = Engine::NetworkManager::CreateClient(port, ip);
        client = std::make_unique<Client::Client>(clientNetInterface.get(), username);

        if (!client->netInterface->Connected())
            return false;

        Engine::SceneManager::GetSceneRoot()->UnloadAllScenes();

        if (!clientWorld)
        {
            clientWorld = Engine::SceneManager::GetSceneRoot()->AddScene<Client::World>("client_world", true, false);
        }

        clientWorld->InitWorld(client.get());

        return true;
    }

    bool Game::LoadWorld(const std::string& worldName, const std::string& username)
    {
        Engine::NetworkManager::CreateLocalNetwork(serverNetInterface, clientNetInterface);

        server = std::make_unique<Server::Server>(serverNetInterface.get());
        client = std::make_unique<Client::Client>(clientNetInterface.get(), username);

        if (!client->netInterface->Connected())
            return false;

        Engine::SceneManager::GetSceneRoot()->UnloadAllScenes();

        if (!serverWorld)
        {
            serverWorld = Engine::SceneManager::GetSceneRoot()->AddScene<Server::World>("server_world", true, false);
        }

        if (!clientWorld)
        {
            clientWorld = Engine::SceneManager::GetSceneRoot()->AddScene<Client::World>("client_world", true, false);
        }

        serverWorld->InitWorld(server.get(), worldName);
        clientWorld->InitWorld(client.get());

        return true;
    }

    void Game::Disconnect()
    {
        pendingDisconnect = true;
    }
}

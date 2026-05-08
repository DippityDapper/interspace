#include "interspace/shared/world/UniverseManager.hpp"

#include "igneous/scenes/SceneRoot.hpp"

namespace Interspace
{
    void UniverseManager::OnCreated()
    {
        serverUniverse = root->AddScene<Server::ServerUniverse>("server_universe");
        clientUniverse = root->AddScene<Client::ClientUniverse>("client_universe");
    }
}
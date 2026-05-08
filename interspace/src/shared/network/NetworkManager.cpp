#include "interspace/shared/network/NetworkManager.hpp"

#include "igneous/networking/LocalIdentity.hpp"
#include "igneous/networking/LocalNetwork.hpp"
#include "igneous/networking/RemoteNetwork.hpp"
#include "igneous/networking/SteamIdentity.hpp"
#include "igneous/networking/SteamNetwork.hpp"
#include "interspace/shared/network/SteamManager.hpp"

namespace Interspace
{
    void NetworkManager::Update(double delta)
    {
        if (server && server->netInterface)
            server->netInterface->Poll();
        if (client && client->netInterface)
            client->netInterface->Poll();
    }

    void NetworkManager::Clean()
    {
        if (server)
        {
            server->netInterface->Clean();
            server->identity->Clean();
            server = nullptr;
        }

        if (client)
        {
            client->netInterface->Clean();
            client->identity->Clean();
            client = nullptr;
        }

        SteamManager::Clean();
    }
    // -------------------------------------------------------------------------
    // Local
    // -------------------------------------------------------------------------

    void NetworkManager::CreateLocalClientServer()
    {
        if (server || client)
            return;

        auto serverNet = std::make_unique<Engine::LocalNetwork>(true);
        auto clientNet = std::make_unique<Engine::LocalNetwork>(false);

        serverNet->SetLoopbackPeer(clientNet.get());
        clientNet->SetLoopbackPeer(serverNet.get());

        server = std::make_unique<Server::Server>(
                std::move(serverNet),
                std::make_unique<Engine::LocalIdentity>("data/shared/local_user.txt"));
        client = std::make_unique<Client::Client>(
                std::move(clientNet),
                std::make_unique<Engine::LocalIdentity>("data/shared/local_user.txt"));
    }

    // -------------------------------------------------------------------------
    // ENet
    // -------------------------------------------------------------------------

    void NetworkManager::CreateServer(int maxClients, bool localOnly)
    {
        if (server)
            return;

        auto netInterface = std::make_unique<Engine::RemoteNetwork>(PORT, maxClients, localOnly);
        auto identity = std::make_unique<Engine::LocalIdentity>("data/shared/local_user.txt");
        server = std::make_unique<Server::Server>(std::move(netInterface), std::move(identity));
    }

    void NetworkManager::CreateClient(const std::string& ip)
    {
        if (client)
            return;

        auto netInterface = std::make_unique<Engine::RemoteNetwork>(PORT, ip);
        auto identity = std::make_unique<Engine::LocalIdentity>("data/shared/local_user.txt");
        client = std::make_unique<Client::Client>(std::move(netInterface), std::move(identity));
    }

    void NetworkManager::CreateRemoteClientServer(int maxClients, bool localOnly)
    {
        auto serverNet = std::make_unique<Engine::RemoteNetwork>(PORT, maxClients, localOnly);
        auto clientNet = std::make_unique<Engine::RemoteNetwork>();

        serverNet->SetLoopbackPeer(clientNet.get());
        clientNet->SetLoopbackPeer(serverNet.get());

        server = std::make_unique<Server::Server>(
                std::move(serverNet),
                std::make_unique<Engine::LocalIdentity>("data/shared/local_user.txt"));
        client = std::make_unique<Client::Client>(
                std::move(clientNet),
                std::make_unique<Engine::LocalIdentity>("data/shared/local_user.txt"));
    }

    // -------------------------------------------------------------------------
    // Steam
    // -------------------------------------------------------------------------

    void NetworkManager::CreateSteamServer()
    {
        if (server)
            return;

        auto netInterface = std::make_unique<Engine::SteamNetwork>(PORT, false);
        auto identity = std::make_unique<Engine::SteamIdentity>();
        server = std::make_unique<Server::Server>(std::move(netInterface), std::move(identity));
    }

    void NetworkManager::CreateSteamClient(const std::string& ip)
    {
        if (client)
            return;

        auto netInterface = std::make_unique<Engine::SteamNetwork>(PORT, ip);
        auto identity = std::make_unique<Engine::SteamIdentity>();
        client = std::make_unique<Client::Client>(std::move(netInterface), std::move(identity));
    }

    void NetworkManager::CreateSteamClientServer()
    {
        auto serverNet = std::make_unique<Engine::SteamNetwork>(PORT, false);
        auto clientNet = std::make_unique<Engine::SteamNetwork>();

        serverNet->SetLoopbackPeer(clientNet.get());
        clientNet->SetLoopbackPeer(serverNet.get());

        server = std::make_unique<Server::Server>(
                std::move(serverNet),
                std::make_unique<Engine::SteamIdentity>());
        client = std::make_unique<Client::Client>(
                std::move(clientNet),
                std::make_unique<Engine::SteamIdentity>());
    }
}
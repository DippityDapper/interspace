#include "interspace/shared/network/NetworkManager.hpp"

#include "igneous/networking/ENetNetwork.hpp"
#include "igneous/networking/LocalIdentity.hpp"
#include "igneous/networking/LocalNetwork.hpp"
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

    void NetworkManager::CreateLocalClientServer()
    {
        if (server || client)
            return;

        auto serverNet = std::make_unique<Engine::LocalNetwork>();
        auto clientNet = std::make_unique<Engine::LocalNetwork>();
        serverNet->Connect(true);
        clientNet->Connect(false);

        serverNet->GetLoopback().SetPeer(clientNet.get());
        clientNet->GetLoopback().SetPeer(serverNet.get());

        server = std::make_unique<Server::Server>(
                std::move(serverNet),
                std::make_unique<Engine::LocalIdentity>("data/shared/local_user.txt"));
        client = std::make_unique<Client::Client>(
                std::move(clientNet),
                std::make_unique<Engine::LocalIdentity>("data/shared/local_user.txt"));
    }

    void NetworkManager::CreateServer(int maxClients, bool localOnly)
    {
        if (server)
            return;

        auto netInterface = std::make_unique<Engine::ENetNetwork>();
        netInterface->Connect(PORT, maxClients, localOnly);
        auto identity = std::make_unique<Engine::LocalIdentity>("data/shared/local_user.txt");
        server = std::make_unique<Server::Server>(std::move(netInterface), std::move(identity));
    }

    void NetworkManager::CreateClient(const std::string& ip)
    {
        if (client)
            return;

        auto netInterface = std::make_unique<Engine::ENetNetwork>();
        netInterface->Connect(PORT, ip);
        auto identity = std::make_unique<Engine::LocalIdentity>("data/shared/local_user.txt");
        client = std::make_unique<Client::Client>(std::move(netInterface), std::move(identity));
    }

    void NetworkManager::CreateRemoteClientServer(int maxClients, bool localOnly)
    {
        auto serverNet = std::make_unique<Engine::ENetNetwork>();
        serverNet->Connect(PORT, maxClients, localOnly);
        auto clientNet = std::make_unique<Engine::ENetNetwork>();
        clientNet->Connect();

        serverNet->GetLoopback().SetPeer(clientNet.get());
        clientNet->GetLoopback().SetPeer(serverNet.get());

        server = std::make_unique<Server::Server>(
                std::move(serverNet),
                std::make_unique<Engine::LocalIdentity>("data/shared/local_user.txt"));
        client = std::make_unique<Client::Client>(
                std::move(clientNet),
                std::make_unique<Engine::LocalIdentity>("data/shared/local_user.txt"));
    }

    void NetworkManager::CreateSteamServer()
    {
        if (server)
            return;

        auto netInterface = std::make_unique<Engine::SteamNetwork>();
        netInterface->Connect();
        auto identity = std::make_unique<Engine::SteamIdentity>();
        server = std::make_unique<Server::Server>(std::move(netInterface), std::move(identity));
    }

    void NetworkManager::CreateSteamClient(const std::string& ip)
    {
        if (client)
            return;

        auto netInterface = std::make_unique<Engine::SteamNetwork>();
        netInterface->Connect(std::stoull(ip));
        auto identity = std::make_unique<Engine::SteamIdentity>();
        client = std::make_unique<Client::Client>(std::move(netInterface), std::move(identity));
    }

    void NetworkManager::CreateSteamClientServer()
    {
        auto serverNet = std::make_unique<Engine::SteamNetwork>();
        serverNet->Connect();
        auto clientNet = std::make_unique<Engine::SteamNetwork>();
        clientNet->Connect(0);

        serverNet->GetLoopback().SetPeer(clientNet.get());
        clientNet->GetLoopback().SetPeer(serverNet.get());

        server = std::make_unique<Server::Server>(
                std::move(serverNet),
                std::make_unique<Engine::SteamIdentity>());
        client = std::make_unique<Client::Client>(
                std::move(clientNet),
                std::make_unique<Engine::SteamIdentity>());
    }
}

#include "interspace/client/world/ClientUniverse.hpp"

#include "igneous/networking/Serializer.hpp"
#include "igneous/scenes/SceneRoot.hpp"
#include "interspace/client/menus/MainMenu.hpp"
#include "interspace/shared/network/NetworkManager.hpp"

namespace Interspace::Client
{
    void ClientUniverse::OnCreated()
    {
        NetworkManager::client->ConnectToEvent(SERVER_ID_PACKET, this, &ClientUniverse::OnServerRemotePacket);
        NetworkManager::client->ConnectToEvent(CONNECTION_ACCEPTED, this, &ClientUniverse::OnConnectionAccepted);
        NetworkManager::client->ConnectToEvent(DISCONNECTION_ACKNOWLEDGED, this, &ClientUniverse::OnDisconnectAcknowledged);
        NetworkManager::client->ConnectToEvent(SERVER_DISCONNECTED, this, &ClientUniverse::OnServerDisconnected);

        uint64_t myId = NetworkManager::client->GetMyId();
        std::string username = NetworkManager::client->GetMyUsername();

        if (myId == 0 || username.empty())
        {
            return;
        }

        SendConnectionRequest();
    }

    void ClientUniverse::Update(double delta)
    {
    }

    void ClientUniverse::SendConnectionRequest()
    {
        Engine::Serializer serializer{};
        serializer.Write(static_cast<uint16_t>(SERVER_ID_REQUEST));

        NetworkManager::client->SendToServer(serializer.GetBytes(), ENET_PACKET_FLAG_RELIABLE);
    }

    void ClientUniverse::OnServerRemotePacket(const std::vector<uint8_t>& data)
    {
        Engine::Deserializer deserializer(data);
        uint32_t serverId = deserializer.ReadLong();
        std::vector<uint8_t> token = NetworkManager::client->identity->GetAuthToken(serverId);

        uint64_t myId = NetworkManager::client->GetMyId();
        std::string username = NetworkManager::client->GetMyUsername();

        Engine::Serializer serializer{};
        serializer.Write(static_cast<uint16_t>(CONNECTION_REQUEST));
        serializer.Write(myId);
        serializer.Write(username);
        serializer.Write(token);
        NetworkManager::client->SendToServer(serializer.GetBytes(), ENET_PACKET_FLAG_RELIABLE);
    }

    void ClientUniverse::OnConnectionAccepted(const std::vector<uint8_t>& data)
    {
    }

    void ClientUniverse::OnDisconnectAcknowledged(const std::vector<uint8_t>& data)
    {
        NetworkManager::Clean();
        root->RemoveScenes("universe");
        root->AddScene<MainMenu>("main_menu", "main_menus", true);
    }

    void ClientUniverse::OnServerDisconnected(const std::vector<uint8_t>& data)
    {
        NetworkManager::Clean();
        root->RemoveScenes("universe");
        root->AddScene<MainMenu>("main_menu", "main_menus", true);
    }
}
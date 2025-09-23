#include "server/networking/NetworkManager.h"

namespace Engine
{
    std::map<PacketType, std::vector<std::function<void(ENetEvent&)>>> NetworkManager::packetHandlers{};
    std::vector<std::function<void(int, NetworkManager*)>> NetworkManager::connectionEvent;
    std::vector<std::function<void(int, NetworkManager*)>> NetworkManager::disconnectionEvent;

    SDL_AppResult NetworkManager::Init()
    {
        if (enet_initialize() != 0)
        {
            SDL_Log("An error occurred while initializing ENet: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        InitPacketHandlers();

        return SDL_APP_CONTINUE;
    }

    void NetworkManager::InitPacketHandlers()
    {
        AddHandler(PACKET_CONNECT, [this](ENetEvent& event) { HandleConnectionPacket(event); });
        AddHandler(PACKET_DISCONNECT, [this](ENetEvent& event) { HandleDisconnectPacket(event); });
    }

    void NetworkManager::AddHandler(PacketType pType, const std::function<void(ENetEvent &)>& func)
    {
        packetHandlers[pType].emplace_back(func);
    }

    SDL_AppResult NetworkManager::CreateServer(int port, bool localHostOnly)
    {
        ENetAddress address;

        if (localHostOnly)
        {
            enet_address_set_host(&address, "localhost");
            SDL_Log("The server is running locally");
        }
        else
        {
            enet_address_set_host(&address, "0.0.0.0");
            SDL_Log("The server is running publicly");
        }

        address.port = port;

        server = enet_host_create(&address, 32, 2, 0, 0);
        if (!server)
        {
            SDL_Log("An error occurred while trying to create an ENet server host: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        SDL_Log("Server started on port %d", address.port);
        return SDL_APP_CONTINUE;
    }

    void NetworkManager::DisconnectClient(int clientId)
    {
        if (peers.contains(clientId))
        {
            peers.erase(clientId);
            SDL_Log("Peer removed: %d", clientId);

            ClientDataPacket clientDataPacket;
            clientDataPacket.type = PACKET_DISCONNECT;
            clientDataPacket.clientId = clientId;

            ENetPacket* packet = enet_packet_create(&clientDataPacket, sizeof(ClientDataPacket), ENET_PACKET_FLAG_RELIABLE);
            enet_host_broadcast(server, 0, packet);
            SDL_Log("Peer disconnected: %d", clientId);
        }
    }

    void NetworkManager::HandleNetworkEvents()
    {
        ENetEvent enetEvent;
        while (enet_host_service(server, &enetEvent, 0) > 0)
        {
            switch (enetEvent.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                {
                    HandlePackets(enetEvent);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT:
                {
                    for (auto kvp: std::map(peers))
                    {
                        ENetPeer* peer = kvp.second;
                        int clientId = kvp.first;
                        if (peer == enetEvent.peer)
                        {
                            SDL_Log("Unexpected disconnect for client : %u", clientId);
                            DisconnectClient(clientId);
                            break;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    void NetworkManager::HandlePackets(ENetEvent enetEvent)
    {
        auto* p_packetType = reinterpret_cast<PacketType*>(enetEvent.packet->data);
        PacketType packetType = *p_packetType;

        if (packetHandlers.contains(packetType))
        {
            std::vector<std::function<void(ENetEvent&)>> callers = packetHandlers[packetType];
            for (auto& caller : callers)
            {
                caller(enetEvent);
            }
        }
        else
        {
            SDL_Log("Unhandled packet type: %d", packetType);
        }

        enet_packet_destroy(enetEvent.packet);
    }

    void NetworkManager::HandleConnectionPacket(ENetEvent &enetEvent)
    {
        ConnectPacket* connectPacket = (ConnectPacket*)enetEvent.packet->data;
        std::string userName = connectPacket->username;
        int clientId = Game::GameData::GetUserNumber(userName);

        if (clientId < 0)
        {
            bool invalidId = true;
            while (invalidId)
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> distrib(0, 100000);

                clientId = distrib(gen);

                if (!peers.contains(clientId))
                {
                    invalidId = false;
                }
            }
            Game::GameData::AddUser(clientId, userName);
        }

        ClientDataPacket clientDataPacket;
        clientDataPacket.type = PACKET_CLIENT_DATA;

        peers[clientId] = enetEvent.peer;
        for (auto& onConnectionCalls : connectionEvent)
        {
            onConnectionCalls(clientId, this);
        }
        clientDataPacket.clientId = clientId;

        SDL_Log("New Client was given id: %d", clientDataPacket.clientId);

        ENetPacket* packet = enet_packet_create(&clientDataPacket, sizeof(ClientDataPacket), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(enetEvent.peer, 0, packet);
    }

    void NetworkManager::HandleDisconnectPacket(ENetEvent &enetEvent)
    {
        DisconnectPacket* disconnectPacket = (DisconnectPacket*)enetEvent.packet->data;
        int clientId = disconnectPacket->clientId;

        for (auto& caller : disconnectionEvent)
        {
            caller(clientId, this);
        }

        DisconnectClient(clientId);
    }
}

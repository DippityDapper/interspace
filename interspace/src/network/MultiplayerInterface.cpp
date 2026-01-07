#include "interspace/network/MultiplayerInterface.hpp"

#include "interspace/network/NetworkPackets.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"

namespace Interspace
{
    MultiplayerInterface::MultiplayerInterface(int port, int peerCount, bool localOnly)
    {
        isServer = true;

        ENetAddress address;

        address.port = port;
        if (localOnly)
            enet_address_set_host_ip(&address, "127.0.0.1");
        else
            address.host = ENET_HOST_ANY;

        host = enet_host_create(&address, peerCount, 2, 0, 0);
        if (!host)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create server: %s.", SDL_GetError());
            return;
        }

        running = true;
        networkThread = std::thread(&MultiplayerInterface::NetworkLoop, this);
    }

    MultiplayerInterface::MultiplayerInterface(int port, const std::string& ip)
    {
        isServer = false;

        host = enet_host_create(nullptr, 1, 2, 0, 0);

        if (!host)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create client: %s.", SDL_GetError());
            enet_host_destroy(host);
            host = nullptr;
            return;
        }

        ENetAddress address;

        enet_address_set_host_ip(&address, ip.c_str());
        address.port = port;

        serverPeer = enet_host_connect(host, &address, 2, 0);
        if (!serverPeer)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create peer: %s.", SDL_GetError());
            enet_host_destroy(host);
            host = nullptr;
            return;
        }

        ENetEvent event;
        if (!(enet_host_service(host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to connect to server.");
            enet_host_destroy(host);
            host = nullptr;
            serverPeer = nullptr;
            return;
        }

        running = true;
        networkThread = std::thread(&MultiplayerInterface::NetworkLoop, this);
    }

    MultiplayerInterface::~MultiplayerInterface()
    {
        running = false;
        if (networkThread.joinable())
            networkThread.join();

        if (host)
            enet_host_destroy(host);
    }

    void MultiplayerInterface::NetworkLoop()
    {
        while (running)
        {
            ENetEvent event;
            while (enet_host_service(host, &event, 1) > 0)
            {
                if (isServer)
                    HandleServerEvent(event);
                else
                    HandleClientEvent(event);
            }

            while (const auto& packet = toNetwork.Pop())
            {
                if (isServer)
                    SendPacket(packet->peer, packet->data, packet->packetType);
                else if (serverPeer)
                    SendPacket(serverPeer, packet->data, packet->packetType);
            }

            enet_host_flush(host);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void MultiplayerInterface::HandleServerEvent(const ENetEvent& event)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            {
                std::vector<uint8_t> msg(event.packet->data, event.packet->data + event.packet->dataLength);
                fromNetwork.Push({event.peer, msg});
                enet_packet_destroy(event.packet);
                break;
            }
        case ENET_EVENT_TYPE_CONNECT:
            {
                std::vector<uint8_t> msg;
                msg.push_back(CONNECTION_REQUEST_);
                fromNetwork.Push({event.peer, msg});
                break;
            }
        case ENET_EVENT_TYPE_DISCONNECT:
            {
                std::vector<uint8_t> msg;
                msg.push_back(DISCONNECTION_REQUEST_);
                fromNetwork.Push({event.peer, msg});
                break;
            }
        default:
            break;
        }
    }

    void MultiplayerInterface::HandleClientEvent(const ENetEvent& event)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            {
                std::vector<uint8_t> msg(event.packet->data, event.packet->data + event.packet->dataLength);
                fromNetwork.Push({event.peer, msg});
                enet_packet_destroy(event.packet);
                break;
            }
        case ENET_EVENT_TYPE_CONNECT:
            {
                break;
            }
        case ENET_EVENT_TYPE_DISCONNECT:
            {
                std::vector<uint8_t> msg;
                msg.push_back(DISCONNECTION_REQUEST_);
                fromNetwork.Push({event.peer, msg});
                break;
            }
        default:
            break;
        }
    }

    void MultiplayerInterface::SendPacket(ENetPeer* peer, std::vector<uint8_t> data, enet_uint32 packetType)
    {
        ENetPacket* enetPacket = enet_packet_create(data.data(), data.size(), packetType);
        enet_peer_send(peer, 0, enetPacket);
    }

    void MultiplayerInterface::Poll()
    {
        while (const auto& msg = fromNetwork.Pop())
        {
            if (isServer && serverMessageHandler != nullptr)
                serverMessageHandler(msg->data, msg->peer);
            else if (clientMessageHandler != nullptr)
                clientMessageHandler(msg->data);
        }
    }

    bool MultiplayerInterface::Connected()
    {
        return running;
    }

    void MultiplayerInterface::SendToClient(ENetPeer* peer, std::vector<uint8_t> data, enet_uint32 packetType)
    {
        if (!isServer)
            return;

        ENetPacket* packet = enet_packet_create(data.data(), data.size(), packetType);
        enet_peer_send(peer, 0, packet);
    }

    void MultiplayerInterface::SendToServer(std::vector<uint8_t> data, enet_uint32 packetType)
    {
        if (isServer || !serverPeer)
            return;

        ENetPacket* packet = enet_packet_create(data.data(), data.size(), packetType);
        enet_peer_send(serverPeer, 0, packet);
    }
}

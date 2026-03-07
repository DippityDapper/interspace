#include "SDL3/SDL_log.h"
#include "igneous/networking/Serializer.hpp"

#include <string>

#include "interspace/client/Client.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/network/NetworkPackets.hpp"

#include <random>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Client
    {
        void Client::RequestConnection(const std::string& _username)
        {
            std::vector<uint8_t> msg{CONNECTION_REQUEST};

            Engine::Serializer serializer(msg);
            serializer << _username;

            netInterface->SendToServer(msg, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Server
    {
        void Server::OnConnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            std::string username{};
            Engine::Deserializer deserializer(data);
            deserializer >> username;

            client_id_t newId = ConnectClient(from, username);

            AcceptConnectionRequest(from, newId);
            BroadcastConnectionToPeers(newId, username);
            SendPeersToPeer(from);

            SDL_Log("[Server] User %s [%u] connected.", username.c_str(), newId);

            std::vector<uint8_t> newClientData{CLIENT_CONNECTED};
            Engine::Serializer broadcastSerializer(newClientData);
            broadcastSerializer << newId << username;
            EmitEvent(CLIENT_CONNECTED, newClientData, from);
        }
    }

    //----------------------------
    // UNREQUESTED
    //----------------------------
    namespace Server
    {
        void Server::OnUnRequestedConnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            client_id_t clientId = GetClientId(from);
            if (clientId != 0)
                return;

            if (connectionAttempts.contains(from))
                return;

            uint64_t currentTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            uint64_t retryTime = currentTime + timeoutOffsetInSeconds;

            ConnectionAttempt connectionAttempt{retryTime, 0};
            connectionAttempts.emplace(from, connectionAttempt);
        }
    }
}
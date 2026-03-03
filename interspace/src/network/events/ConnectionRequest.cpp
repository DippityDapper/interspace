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

            if (!DBHelper::PlayerExists(username))
            {
                std::mt19937 gen(std::random_device{}());

                int peerId = 0;
                do
                {
                    std::uniform_int_distribution<> peerIdDist(1, INT32_MAX);
                    peerId = peerIdDist(gen);
                } while (DBHelper::PlayerExists(peerId));

                DBHelper::InsertPlayer(peerId, username);
            }

            uint32_t newId = DBHelper::GetPlayerId(username);

            AcceptConnectionRequest(from, newId);
            BroadcastConnectionToPeers(newId, username);
            SendPeersToPeer(from);

            peers.emplace(newId, from);
            idToUsernameLookup.emplace(newId, username);

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
        }
    }
}
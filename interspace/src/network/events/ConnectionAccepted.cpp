#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Server
    {
        void Server::AcceptConnectionRequest(_ENetPeer* to, uint32_t id)
        {
            std::vector<uint8_t> response{CONNECTION_ACCEPTED};
            Engine::Serializer responseSerializer(response);
            responseSerializer << id;
            netInterface->SendToClient(to, response, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Client
    {
        void Client::OnConnectionAccepted(const std::vector<uint8_t>& data)
        {
            Engine::Deserializer deserializer(data);
            deserializer >> clientId;
        }
    }
}
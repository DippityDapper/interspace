#include "interspace/client/Client.hpp"

#include "interspace/game/Game.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/network/Serializer.hpp"

namespace Interspace::Client
{
    void Client::HandleConnectionAccepted(const std::vector<uint8_t>& data)
    {
        Deserializer deserializer(data);
        deserializer >> clientId;
    }

    void Client::HandleClientDisconnected(const std::vector<uint8_t>& data)
    {
        uint32_t peerId = 0;
        Deserializer deserializer(data);
        deserializer >> peerId;

        std::string peerUsername = peers[peerId];

        if (peers.contains(peerId))
            peers.erase(peerId);
    }

    void Client::HandlePeerConnected(const std::vector<uint8_t>& data)
    {
        uint32_t peerId = 0;
        std::string peerUsername{};

        Deserializer deserializer(data);
        deserializer
            >> peerId
            >> peerUsername;

        peers[peerId] = peerUsername;
    }
}

#include "game/client/Client.hpp"

#include "game/game/Game.hpp"
#include "game/network/NetworkPackets.hpp"

namespace Game::Client
{
    void Client::HandleConnectionAccepted(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;
        clientId = UnpackUint32(data, offset);
    }

    void Client::HandleClientDisconnected(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;

        uint32_t peerId = UnpackUint32(data, offset);
        std::string peerUsername = peers[peerId];

        if (peers.contains(peerId))
            peers.erase(peerId);
    }

    void Client::HandlePeerConnected(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;

        uint32_t peerId =  UnpackUint32(data, offset);

        uint32_t usernameLen = UnpackUint32(data, offset);
        std::string peerUsername = UnpackString(data, offset, usernameLen);

        peers[peerId] = peerUsername;
    }
}

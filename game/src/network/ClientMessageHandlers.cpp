#include <cstring>

#include "SDL3/SDL_log.h"

#include "game/network/Client.hpp"
#include "game/game/Game.hpp"
#include "game/network/NetworkPackets.hpp"

namespace Game
{
    void Client::HandleConnectionAccepted(const std::vector<uint8_t>& data)
    {
        uint32_t offset = 1;
        std::memcpy(&clientId, &data[offset], sizeof(uint32_t));
        SDL_Log("[Client] Connected. Assigned ID: %u", clientId);
    }

    void Client::HandleClientDisconnected(const std::vector<uint8_t>& data)
    {
        uint32_t offset = 1;
        uint32_t peerId;
        std::memcpy(&peerId, &data[offset], sizeof(uint32_t));

        std::string peerUsername = peers[peerId];

        if (peers.contains(peerId))
            peers.erase(peerId);
    }

    void Client::HandlePeerConnected(const std::vector<uint8_t>& data)
    {
        uint32_t offset = 1;

        uint32_t peerId;
        std::memcpy(&peerId, &data[offset], sizeof(uint32_t));
        offset += sizeof(uint32_t);

        uint32_t usernameLen = 0;
        std::memcpy(&usernameLen, &data[offset], sizeof(uint32_t));
        offset += sizeof(uint32_t);
        std::string peerUsername(reinterpret_cast<const char*>(&data[offset]), usernameLen);

        peers[peerId] = peerUsername;
        SDL_Log("[Client] Peer Connected: %s [%u]", peerUsername.c_str(), peerId);
    }
}

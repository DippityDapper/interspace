#pragma once

#include "interspace/server/Server.hpp"
#include "igneous/scenes/Scene.hpp"

#include <memory>

namespace Interspace::Server
{
    struct AuthPeer
    {
        uint32_t peerId = 0;
        std::string username{};
    };

    class ServerUniverse : public Engine::Scene
    {
      private:
        std::unordered_map<uint64_t, AuthPeer> pendingAuth{};

      public:
        void OnCreated() override;

        void OnServerRemoteIdRequest(const std::vector<uint8_t>& data, uint32_t peerId);
        void OnClientConnectionRequest(const std::vector<uint8_t>& data, uint32_t peerId);
        void OnClientDisconnectionRequest(const std::vector<uint8_t>& data, uint32_t peerId);

        void OnAuthResult(uint64_t clientId, bool valid);
        void RejectClient(uint32_t peerId, uint64_t clientId);
    };
}

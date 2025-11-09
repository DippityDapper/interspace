#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>

#include "enet/enet.h"

namespace Game
{
    class NetInterface;

    class Server
    {
    private:
        static inline NetInterface* netInterface = nullptr;

        static inline std::unordered_map<uint32_t, ENetPeer*> peers{};
        static inline uint32_t nextPeerId = 0;

    public:
        static void SetNetInterface(NetInterface* _netInterface);
        static void HandleMessage(std::vector<uint8_t> data, ENetPeer* peer);
    };
}
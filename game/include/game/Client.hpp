#pragma once

#include <cstdint>
#include <vector>

namespace Game
{
    class NetInterface;

    class Client
    {
    private:
        static inline NetInterface* netInterface = nullptr;

        static inline uint32_t clientId = 0;

    public:
        static void SetNetInterface(NetInterface* _netInterface);
        static void HandleMessage(std::vector<uint8_t>);
    };
}

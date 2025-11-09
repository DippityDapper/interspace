#include "game/Client.hpp"

#include <cstring>
#include <string>

#include "SDL3/SDL_log.h"

#include "game/NetworkPackets.hpp"

namespace Game
{
    void Client::SetNetInterface(NetInterface* _netInterface)
    {
        netInterface = _netInterface;
    }

    void Client::HandleMessage(std::vector<uint8_t> data)
    {
        if (data.empty())
            return;
        if (!netInterface)
            return;

        uint8_t type = data[0];

        switch (type)
        {
        case CONNECTION_ACCEPTED:
            {
                std::memcpy(&clientId, data.data() + 1, sizeof(uint32_t));
                SDL_Log("Connected. Assigned ID: %u", clientId);
                break;
            }
        default:
            break;
        }
    }
}

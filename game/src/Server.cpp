#include "game/Server.hpp"

#include <string>

#include "game/NetInterface.hpp"
#include "SDL3/SDL_log.h"

#include "game/NetworkPackets.hpp"

namespace Game
{
    void Server::SetNetInterface(NetInterface* _netInterface)
    {
        netInterface = _netInterface;
    }

    void Server::HandleMessage(std::vector<uint8_t> data, ENetPeer* peer)
    {
        if (data.empty())
            return;
        if (!netInterface)
            return;

        uint8_t type = data[0];

        switch (type)
        {
        case CONNECTION_REQUEST:
            {
                uint32_t newId = nextPeerId++;
                peers.emplace(newId, peer);

                std::vector<uint8_t> response;
                response.push_back(CONNECTION_ACCEPTED);

                uint8_t* idBytes = reinterpret_cast<uint8_t*>(&newId);
                response.insert(response.end(), idBytes, idBytes + sizeof(uint32_t));

                netInterface->SendToClient(peer, response);

                break;
            }

        default:
            SDL_Log("Unknown packet type: %u", type);
            break;
        }
    }
}

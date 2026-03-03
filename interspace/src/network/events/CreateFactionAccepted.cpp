#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "igneous/scenes/SceneManager.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/World.hpp"
#include "interspace/menus/CreateFactionMenu.hpp"
#include "interspace/server/World.hpp"

#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Server
    {
        void World::AcceptFactionRequest(ENetPeer* peer)
        {
            std::vector<uint8_t> acceptedData{CREATE_FACTION_ACCEPTED};
            server->netInterface->SendToClient(peer, acceptedData, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    void CreateFactionMenu::OnFactionAccepted(const std::vector<uint8_t>& data)
    {
        SetActive(false);
        awaitingResponse = false;
    }
}
#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "igneous/scenes/SceneManager.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/ClientWorld.hpp"
#include "interspace/menus/CreateFactionMenu.hpp"
#include "interspace/server/ServerWorld.hpp"

#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Server
    {
        void ServerWorld::DenyFactionRequest(ENetPeer* peer)
        {
            std::vector<uint8_t> deniedData{CREATE_FACTION_DENIED};

            std::string errorMessage{"Faction already exists."};
            Engine::Serializer serializer(deniedData);
            serializer << errorMessage;

            server->netInterface->SendToClient(peer, deniedData, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    void CreateFactionMenu::OnFactionDenied(const std::vector<uint8_t>& data)
    {
        Engine::Deserializer deserializer(data);
        deserializer >> errorMessage;

        awaitingResponse = false;
    }
}
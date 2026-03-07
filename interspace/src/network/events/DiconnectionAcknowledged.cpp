#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "igneous/scenes/SceneManager.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/ClientWorld.hpp"
#include "interspace/game/Game.hpp"

#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Server
    {
        void Server::AcknowledgeDisconnection(ENetPeer* to)
        {
            std::vector<uint8_t> acknowledgment{DISCONNECTION_ACKNOWLEDGED};
            netInterface->SendToClient(to, acknowledgment, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Client
    {
        void ClientWorld::OnDisconnectAcknowledged(const std::vector<uint8_t>& data)
        {
            Game::Disconnect();

            if (Engine::SceneManager::GetSceneRoot()->SceneExists("main_menu"))
                Engine::SceneManager::GetSceneRoot()->LoadScene("main_menu");
        }
    }
}

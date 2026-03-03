#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "igneous/scenes/SceneManager.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/World.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/menus/CreateFactionMenu.hpp"
#include "interspace/server/World.hpp"

#include <ranges>
#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Client
    {
        void World::SendPosition()
        {
            std::vector<uint8_t> data{PLAYER_POSITION_PACKET};
            Engine::Serializer serializer{data};
            serializer << client->clientId << camera->position.x << camera->position.y;
            client->netInterface->SendToServer(data, 0);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Server
    {
        void World::OnPlayerPositionReceived(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            Engine::Deserializer deserializer(data);

            uint32_t clientId = 0;
            float positionX, positionY;
            deserializer >> clientId >> positionX >> positionY;
            if (!players.contains(clientId))
                return;
            Player* player = players[clientId].get();

            player->position = {positionX, positionY};
        }
    }
}
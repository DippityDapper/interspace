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
    void CreateFactionMenu::SendCreationRequest(const std::string& factionName)
    {
        std::vector<uint8_t> data{CREATE_FACTION_REQUEST};
        Engine::Serializer serializer(data);

        serializer << client->clientId << factionName;

        errorMessage = "";
        client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
    }

    namespace Server
    {
        void World::SendFactionCreateRequest(uint32_t clientId)
        {
            std::vector<uint8_t> data{CREATE_FACTION_REQUEST};
            ENetPeer* peer = server->GetPeer(clientId);
            server->netInterface->SendToClient(peer, data, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Server
    {
        void World::OnCreateFactionRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            uint32_t playerId = 0;
            std::string factionName{};

            Engine::Deserializer deserializer(data);
            deserializer >> playerId >> factionName;

            uint16_t factionId = AddFaction(factionName, playerId);
            if (factionId == 0)
            {
                DenyFactionRequest(from);
            }
            else
            {
                std::string playerName = server->GetUsername(playerId);
                AddColonistToFaction(factionId, playerName);
                AcceptFactionRequest(from);
            }
        }
    }

    namespace Client
    {
        void World::OnCreateFactionRequestReceived(const std::vector<uint8_t>& data)
        {
            Engine::SceneRoot* sceneRoot = Engine::SceneManager::GetSceneRoot();
            if (sceneRoot->SceneExists("create_faction_menu"))
            {
                sceneRoot->LoadScene("create_faction_menu");
            }
            else
            {
                sceneRoot->AddScene<CreateFactionMenu>("create_faction_menu");
            }
        }
    }
}
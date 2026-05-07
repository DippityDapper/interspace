#pragma once

#include "SQLiteCpp/Database.h"
#include "enet/enet.h"
#include "interspace/shared/game/Typedefs.hpp"
#include "interspace/shared/world/PlayerData.hpp"

#include <string>
#include <unordered_map>

namespace Interspace
{
    class ClientManager
    {
      private:
        static inline std::unordered_map<client_id_t, ENetPeer*> peers{};
        static inline std::unordered_map<client_id_t, std::string> idToUsername{};
        static inline std::unordered_map<std::string, client_id_t> usernameToId{};
        static inline std::unordered_map<ENetPeer*, client_id_t> peerToId{};
        static inline std::unordered_map<client_id_t, std::unique_ptr<PlayerData>> playerData{};

      public:
        static void Init();
        static void CreateTables();

        static bool InsertClient(client_id_t clientId, const std::string& username);
        static bool DeleteClient(client_id_t clientId);

        static void AddClient(client_id_t clientId, const std::string& username, ENetPeer* peer);
        static void RemoveClient(client_id_t clientId);

        static bool ClientExists(const std::string& username);
        static bool ClientExists(client_id_t clientId);

        static bool PeerExists(client_id_t clientId);

        static client_id_t GetClientId(const std::string& username);
        static client_id_t GetClientId(ENetPeer* peer);

        static std::string GetClientUsername(client_id_t clientId);
        static std::string GetClientUsername(ENetPeer* peer);

        static ENetPeer* GetClientPeer(client_id_t clientId);
        static ENetPeer* GetClientPeer(const std::string& username);

        static std::unordered_map<client_id_t, ENetPeer*> GetPeers();

        static bool InsertPlayerData(client_id_t clientId, world_id_t worldId, universe_id_t universeId);
        static bool DeletePlayerData(client_id_t clientId);
        static bool UpdatePlayerData(client_id_t clientId, float posX, float posY);
        static bool PlayerDataExists(client_id_t clientId);

        static void SetPlayerData(client_id_t clientId, float posX, float posY);
        static PlayerData* GetPlayerData(client_id_t clientId);
    };
}
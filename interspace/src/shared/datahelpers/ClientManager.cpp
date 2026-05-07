#include "interspace/shared/datahelpers/ClientManager.hpp"

#include "interspace/shared/datahelpers/DatabaseManager.hpp"

namespace Interspace
{
    void ClientManager::Init()
    {
        if (!std::filesystem::exists("data"))
            std::filesystem::create_directory("data");
        if (!std::filesystem::exists("data/shared"))
            std::filesystem::create_directory("data/shared");

        CreateTables();

        SQLite::Database* db = DatabaseManager::GetSharedDatabase();
        SQLite::Statement query(*db, "SELECT clientId, clientUsername FROM client");

        while (query.executeStep())
        {
            client_id_t clientId = query.getColumn(0).getInt();
            std::string username = query.getColumn(1).getString();

            usernameToId.emplace(username, clientId);
            idToUsername.emplace(clientId, username);
        }
    }

    void ClientManager::CreateTables()
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        db->exec(R"(
            CREATE TABLE IF NOT EXISTS client(
                clientId INTEGER PRIMARY KEY,
                clientUsername VARCHAR(255) NOT NULL UNIQUE,
                createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now'))
            );
        )");

        db->exec(R"(
            CREATE TABLE IF NOT EXISTS player(
                clientId INTEGER PRIMARY KEY,
                firstTimeJoined BOOLEAN DEFAULT 1,
                FOREIGN KEY (clientId) REFERENCES client(clientId) ON DELETE CASCADE
            );
        )");

        db->exec(R"(
            CREATE TABLE IF NOT EXISTS playerData(
                clientId INTEGER PRIMARY KEY,
                worldId INTEGER NOT NULL,
                universeId INTEGER NOT NULL,
                posX REAL DEFAULT 0,
                posY REAL DEFAULT 0,
                FOREIGN KEY (worldId, universeId) REFERENCES world(worldId, universeId) ON DELETE CASCADE,
                FOREIGN KEY (clientId) REFERENCES client(clientId) ON DELETE CASCADE
            );
        )");
    }

    bool ClientManager::ClientExists(const std::string& username)
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();
        SQLite::Statement query(*db, "SELECT clientId FROM client WHERE clientUsername = ?");
        query.bind(1, username);
        return query.executeStep();
    }

    bool ClientManager::ClientExists(client_id_t clientId)
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();
        SQLite::Statement query(*db, "SELECT clientId FROM client WHERE clientId = ?");
        query.bind(1, clientId);
        return query.executeStep();
    }

    bool ClientManager::PeerExists(client_id_t clientId)
    {
        if (!peers.contains(clientId))
            return false;
        return true;
    }

    bool ClientManager::InsertClient(client_id_t clientId, const std::string& username)
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement statement(*db, R"(
            INSERT INTO client(clientId, clientUsername)
            VALUES(?, ?)
        )");

        statement.bind(1, clientId);
        statement.bind(2, username);

        return statement.exec() > 0;
    }

    bool ClientManager::DeleteClient(client_id_t clientId)
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement query(*db, "DELETE FROM client WHERE clientId = ?");
        query.bind(1, clientId);
        return query.exec() > 0;
    }

    void ClientManager::AddClient(client_id_t clientId, const std::string& username, ENetPeer* peer)
    {
        if (idToUsername.contains(clientId))
            return;

        idToUsername.emplace(clientId, username);
        usernameToId.emplace(username, clientId);
        peers.emplace(clientId, peer);
        peerToId.emplace(peer, clientId);
    }

    void ClientManager::RemoveClient(client_id_t clientId)
    {
        if (!idToUsername.contains(clientId))
            return;

        std::string username = idToUsername[clientId];
        ENetPeer* peer = peers[clientId];

        idToUsername.erase(clientId);
        usernameToId.erase(username);
        peers.erase(clientId);
        peerToId.erase(peer);
    }

    client_id_t ClientManager::GetClientId(const std::string& username)
    {
        if (usernameToId.contains(username))
            return usernameToId[username];

        SQLite::Database* db = DatabaseManager::GetSharedDatabase();
        SQLite::Statement query(*db, "SELECT clientId FROM client WHERE clientUsername = ?");
        query.bind(1, username);

        if (query.executeStep())
        {
            return static_cast<client_id_t>(query.getColumn(0).getInt());
        }
        return 0;
    }

    client_id_t ClientManager::GetClientId(ENetPeer* peer)
    {
        if (!peerToId.contains(peer))
            return 0;
        return peerToId[peer];
    }

    std::string ClientManager::GetClientUsername(client_id_t clientId)
    {
        if (idToUsername.contains(clientId))
            return idToUsername[clientId];

        SQLite::Database* db = DatabaseManager::GetSharedDatabase();
        SQLite::Statement query(*db, "SELECT clientUsername FROM client WHERE clientId = ?");
        query.bind(1, clientId);

        if (query.executeStep())
        {
            return query.getColumn(0).getString();
        }

        return "";
    }

    std::string ClientManager::GetClientUsername(ENetPeer* peer)
    {
        if (!peerToId.contains(peer))
            return "";
        return GetClientUsername(peerToId[peer]);
    }

    ENetPeer* ClientManager::GetClientPeer(client_id_t clientId)
    {
        if (!peers.contains(clientId))
            return nullptr;
        return peers[clientId];
    }

    ENetPeer* ClientManager::GetClientPeer(const std::string& username)
    {
        if (!usernameToId.contains(username))
            return nullptr;
        return GetClientPeer(usernameToId[username]);
    }

    std::unordered_map<client_id_t, ENetPeer*> ClientManager::GetPeers()
    {
        std::unordered_map<client_id_t, ENetPeer*> result{};
        for (auto& peer: peers)
            result.emplace(peer.first, peer.second);
        return result;
    }

    bool ClientManager::InsertPlayerData(client_id_t clientId, world_id_t worldId, universe_id_t universeId)
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement statement(*db, R"(
            INSERT INTO playerData(clientId, worldId, universeId)
            VALUES(?, ?, ?)
        )");

        statement.bind(1, clientId);
        statement.bind(2, worldId);
        statement.bind(3, universeId);

        return statement.exec() > 0;
    }

    bool ClientManager::DeletePlayerData(client_id_t clientId)
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement query(*db, "DELETE FROM playerData WHERE clientId = ?");
        query.bind(1, clientId);
        return query.exec() > 0;
    }

    bool ClientManager::UpdatePlayerData(client_id_t clientId, float posX, float posY)
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement statement(*db, R"(
            UPDATE playerData
            SET posX = ?, posY = ?
            WHERE clientId = ?
        )");

        statement.bind(1, posX);
        statement.bind(2, posY);
        statement.bind(3, clientId);

        return statement.exec() > 0;
    }

    bool ClientManager::PlayerDataExists(client_id_t clientId)
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();
        SQLite::Statement query(*db, "SELECT clientId FROM playerData WHERE clientId = ?");
        query.bind(1, clientId);
        return query.executeStep();
    }

    void ClientManager::SetPlayerData(client_id_t clientId, float posX, float posY)
    {
        if (!playerData.contains(clientId))
            playerData.emplace(clientId, std::make_unique<PlayerData>());

        PlayerData* data = playerData[clientId].get();
        data->position.x = posX;
        data->position.y = posY;
    }

    PlayerData* ClientManager::GetPlayerData(client_id_t clientId)
    {
        if (!playerData.contains(clientId))
            return nullptr;
        return playerData[clientId].get();
    }
}
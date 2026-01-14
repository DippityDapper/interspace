#include "../../../../../igneous/include/igneous/networking/Serializer.hpp"
#include "interspace/server/World.hpp"

namespace Interspace::Server
{
    void World::OnClientConnected(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        uint32_t clientId = 0;
        std::string username{};
        Engine::Deserializer deserializer(data);
        deserializer >> clientId >> username;

        SendWorldData(from);
        SendFactionData(from);
        SendChunkData(from, clientId);

        JoinFaction(clientId);
    }

    void World::OnClientDisconnected(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        uint32_t clientId = 0;

        Engine::Deserializer deserializer(data);
        deserializer >> clientId;

        BroadcastColonistDeselectAllData(clientId);
    }
}

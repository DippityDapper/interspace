#include "game/server/FactionServer.hpp"

namespace Game
{
    FactionServer::FactionServer(uint32_t _id, uint32_t _ownerId, const std::string& _name)
    {
        id = _id;
        ownerId = _ownerId;
        name = _name;

        std::string newColonistName{"[" + name + "] " + "Dwayne"};
        Engine::Vec2<float> newColonistPosition{0, 0};

        AddColonist(newColonistName, newColonistPosition);
    }

    uint32_t FactionServer::AddColonist(const std::string& colonistName, const Engine::Vec2<float>& position)
    {
        uint32_t colonistId = nextColonistId++;
        colonists.emplace(colonistId, std::make_unique<ColonistServer>(colonistId, colonistName, position));
        colonistIdToName.emplace(colonistId, colonistName);
        colonistNameToId.emplace(colonistName, colonistId);

        return colonistId;
    }
}

#include "game/client/FactionClient.hpp"

namespace Game
{
    FactionClient::FactionClient(uint32_t _id, uint32_t _ownerId, const std::string& _name)
    {
        id = _id;
        name = _name;
        ownerId = _ownerId;

        std::string newColonistName{"[" + name + "] " + "Dwayne"};
        Engine::Vec2<float> newColonistPosition{0, 0};

        AddColonist(newColonistName, newColonistPosition);
    }

    uint32_t FactionClient::AddColonist(const std::string& colonistName, const Engine::Vec2<float>& position)
    {
        uint32_t colonistId = nextColonistId++;
        colonists.emplace(colonistId, std::make_unique<ColonistClient>(colonistId, colonistName, position));
        colonistIdToName.emplace(colonistId, colonistName);
        colonistNameToId.emplace(colonistName, colonistId);

        return colonistId;
    }
}

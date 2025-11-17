#include "game/client/FactionClient.hpp"

namespace Game
{
    FactionClient::FactionClient(uint32_t _id, uint32_t _ownerId, const std::string& _name)
    {
        id = _id;
        ownerId = _ownerId;
        name = _name;

        std::string newColonistName{"Dwayne [" + name + "]"};
        Engine::Vec2<float> newColonistPosition{512*32*64, 512*32*64};

        AddColonist(newColonistName, newColonistPosition);
    }

    void FactionClient::Render()
    {
        for (auto& kvp : colonists)
        {
            ColonistClient* colonist = kvp.second.get();
            colonist->Render();
        }
    }

    uint32_t FactionClient::AddColonist(const std::string& colonistName, const Engine::Vec2<float>& position)
    {
        uint32_t colonistId = nextColonistId++;
        colonists.emplace(colonistId, std::make_unique<ColonistClient>(colonistId, colonistName, position));
        colonistIdToName.emplace(colonistId, colonistName);
        colonistNameToId.emplace(colonistName, colonistId);

        return colonistId;
    }

    std::map<uint32_t,ColonistClient*> FactionClient::GetColonists()
    {
        std::map<uint32_t,ColonistClient*> result;
        for (auto& kvp : colonists)
            result[kvp.first] = kvp.second.get();
        return result;
    }
}

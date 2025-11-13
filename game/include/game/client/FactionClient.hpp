#pragma once

#include <map>
#include <memory>
#include <string>

#include "game/client/ColonistClient.hpp"

namespace Game
{
    class FactionClient
    {
    public:
        uint32_t id = 0;
        uint32_t ownerId = 0;
        std::string name;

        std::map<uint32_t, std::unique_ptr<ColonistClient>> colonists{};
        std::map<uint32_t, std::string> colonistIdToName{};
        std::map<std::string, uint32_t> colonistNameToId{};

        uint32_t nextColonistId = 1;

    public:
        FactionClient(uint32_t _id, uint32_t _ownerId, const std::string& _name);

        uint32_t AddColonist(const std::string& colonistName, const Engine::Vec2<float>& position);
    };
}

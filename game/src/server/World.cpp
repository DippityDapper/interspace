#include "game/server/World.hpp"

#include "igneous/CFGParser.hpp"
#include "game/server/Colonist.hpp"

namespace Game::Server
{
    World::World(Server* _server, const std::string& _worldName)
    {
        server = _server;
        worldName = _worldName;
    }

    void World::Init()
    {
        Engine::CFGParser::LoadConfig("worlds/" + worldName + "/configs.cfg", worldName);

        seed = Engine::CFGParser::GetUInt32(worldName, "world_seed");

        worldData = std::make_unique<WorldData>();
        worldData->worldSizeX = Engine::CFGParser::GetUInt16(worldName, "world_size_x");
        worldData->worldSizeY = Engine::CFGParser::GetUInt16(worldName, "world_size_y");

        RegisterNetEvents();
    }

    void World::Update(float delta)
    {
    }

    void World::Render()
    {
    }

    void World::HandleEvents(SDL_Event& event)
    {
    }

    void World::Clean()
    {
    }

    uint16_t World::AddFaction(uint32_t ownerId)
    {
        uint16_t newFactionId = nextFactionId++;
        factions.emplace(newFactionId, std::make_unique<Faction>());
        Faction* faction = factions[newFactionId].get();

        faction->data.ownerId = ownerId;
        faction->data.id = newFactionId;

        std::string ownerUsername = server->GetUsername(ownerId);
        faction->data.members.emplace(ownerId, ownerUsername);
        faction->data.name = ownerUsername;

        uint16_t newColonistId = nextEntityId++;
        faction->colonists.emplace(newColonistId, std::make_unique<Colonist>());
        Colonist* colonist = faction->colonists[newColonistId].get();

        colonist->entityData.id = newColonistId;
        colonist->entityData.name = "Dwayne [" + faction->data.name + "]";
        faction->colonists.emplace(colonist->entityData.id, std::move(colonist));

        return faction->data.id;
    }
}

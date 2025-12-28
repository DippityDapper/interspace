#include "game/client/World.hpp"

namespace Game::Client
{
    World::World(Client* _client)
    {
        client = _client;
    }

    void World::Init()
    {
        worldData = std::make_unique<WorldData>();
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
}

#include "dapper2d/Engine.hpp"
#include "client/World.hpp"

int main(int argc, char* argv[])
{
    Engine::Engine* engine = new Engine::Engine();

    Game::World* world = new Game::World();
    engine->Init(world);
    engine->Update();
    engine->Clean();

    delete engine;
    return 0;
}
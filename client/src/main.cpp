#include "dapper2d/Engine.hpp"
#include "client/World.hpp"

int main(int argc, char* argv[])
{
    std::unique_ptr<Engine::Engine> engine = std::make_unique<Engine::Engine>();
    std::unique_ptr<Game::World> world = std::make_unique<Game::World>();
    return engine->Run(world.get());
}
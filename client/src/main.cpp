#include "dapper2d/Engine.hpp"
#include "client/WorldCreationScene.hpp"
#include <memory>

int main(int argc, char* argv[])
{
    std::unique_ptr<Engine::Engine> engine = std::make_unique<Engine::Engine>();
    Game::WorldCreationScene* mainScene = new Game::WorldCreationScene();
    return engine->Run(mainScene);
}
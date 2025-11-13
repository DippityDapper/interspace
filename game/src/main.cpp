#include "dapper2d/Engine.hpp"
#include <memory>

#include "dapper2d/Scenes.hpp"
#include "game/game/Game.hpp"

int main(int argc, char* argv[])
{
    std::unique_ptr<Engine::Engine> engine = std::make_unique<Engine::Engine>();
    Game::Game* mainScene = new Game::Game();
    mainScene->singleton = true;
    Engine::Scenes::CreateScene(mainScene, "main");

    return engine->Run();
}

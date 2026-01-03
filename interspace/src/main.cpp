#include "igneous/Engine.hpp"

#include <memory>
#include "igneous/Scenes.hpp"
#include "../include/interspace/game/Game.hpp"

int main(int argc, char* argv[])
{
    std::unique_ptr<Engine::Engine> engine = std::make_unique<Engine::Engine>();
    Interspace::Game* mainScene = new Interspace::Game();
    mainScene->singleton = true;
    Engine::Scenes::CreateScene(mainScene, "main");

    return engine->Run();
}

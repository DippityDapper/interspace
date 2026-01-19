#include "igneous/engine/Engine.hpp"

#include <memory>

#include "interspace/game/Game.hpp"

int main(int argc, char* argv[])
{
    std::unique_ptr<Engine::Engine> engine = std::make_unique<Engine::Engine>();
    return engine->Run<Interspace::Game>("main");
}

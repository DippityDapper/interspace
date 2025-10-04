#include "client/engine/Engine.h"

int main(int argc, char* argv[])
{
    Engine::Engine engine{};

    engine.Init();
    engine.Update();
    engine.Clean();

    return 0;
}
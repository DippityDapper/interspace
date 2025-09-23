#include "client/game/Game.h"

int main(int argc, char* argv[])
{
    Game::Game game{};
    game.Init();

    game.HandleEvents();
    game.Update();
    game.Render();

    game.Clean();

    return 0;
}
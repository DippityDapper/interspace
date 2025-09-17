#include "Game.h"

int main(int argc, char* argv[])
{
    Game game{};
    game.Init();

    while (game.IsRunning())
    {
        game.HandleEvents();
        game.Update();
        game.Render();
    }

    game.Clean();

    return 0;
}
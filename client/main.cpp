#include "Game.h"

int main(int argc, char* argv[])
{
    Game* game = new Game();

    game->Init();

    while (game->IsRunning())
    {
        game->Run();
    }

    game->Quit();

    delete game;
    return 0;
}
#include "Game.h"

int main(int argc, char* argv[])
{
    Game* game = new Game();
    game->Init();

    while (game->IsRunning())
    {
        game->HandleEvents();
        game->Update();
        game->Render();
    }

    game->Clean();

    delete game;
    return 0;
}
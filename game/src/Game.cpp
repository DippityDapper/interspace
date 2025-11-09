#include "game/Game.hpp"

#include "SDL3/SDL_events.h"

#include "dapper2d/Scenes.hpp"

#include "game/MainMenu.hpp"
#include "game/MultiplayerInterface.hpp"

namespace Game
{
    void Game::Init()
    {
        MainMenu* mainMenu = new MainMenu();
        Engine::Scenes::CreateScene(mainMenu, "main_menu");
        Engine::Scenes::LoadScene(mainMenu);
    }

    void Game::Update(float delta)
    {
        // if (netInterface)
        //     netInterface->Poll();
    }

    void Game::HandleEvents(SDL_Event& event)
    {
        // if (event.type == SDL_EVENT_KEY_DOWN)
        // {
        //     switch (event.key.key)
        //     {
        //     case SDLK_1:
        //         {
        //             netInterface = std::make_unique<SingleplayerInterface>();
        //             Server::SetNetInterface(netInterface.get());
        //             Client::SetNetInterface(netInterface.get());
        //             netInterface->serverMessageHandler = &Server::HandleMessage;
        //             netInterface->clientMessageHandler = &Client::HandleMessage;
        //
        //             const std::vector<uint8_t> msg{CONNECTION_REQUEST};
        //             netInterface->SendToServer(msg);
        //
        //             SDL_Log("Singleplayer started.");
        //             break;
        //         }
        //     case SDLK_2:
        //         {
        //             netInterface = std::make_unique<MultiplayerInterface>(33333, 32, true);
        //             Server::SetNetInterface(netInterface.get());
        //             netInterface->serverMessageHandler = &Server::HandleMessage;
        //
        //             SDL_Log("Server started.");
        //             break;
        //         }
        //     case SDLK_3:
        //         {
        //             netInterface = std::make_unique<MultiplayerInterface>(33333, "127.0.0.1");
        //             Client::SetNetInterface(netInterface.get());
        //             netInterface->clientMessageHandler = &Client::HandleMessage;
        //
        //             const std::vector<uint8_t> msg{CONNECTION_REQUEST};
        //             netInterface->SendToServer(msg);
        //
        //             SDL_Log("Client started.");
        //             break;
        //         }
        //     case SDLK_Q:
        //         {
        //             if (netInterface)
        //             {
        //                 std::vector<uint8_t> msg = {'H', 'e', 'l', 'l', 'o', 0};
        //                 netInterface->SendToServer(msg);
        //                 SDL_Log("Sent message to server.");
        //             }
        //             break;
        //         }
        //     case SDLK_E:
        //         if (netInterface)
        //         {
        //             std::vector<uint8_t> msg = {'S', 'e', 'r', 'v', 'e', 'r', '!', 0};
        //             netInterface->SendToClient(nullptr, msg);
        //             SDL_Log("Broadcasted message to client.");
        //             break;
        //         }
        //     default:
        //         break;
        //     }
        // }
    }

    void Game::Clean()
    {

    }
}

#include "client/World.hpp"

#include "imgui.h"
#include "SDL3/SDL.h"

#include "dapper2d/ResourceLoader.hpp"
#include "dapper2d/Window.hpp"

#include "client/Camera.hpp"
#include "client/Area.hpp"

namespace Game
{
    const int World::WORLD_SIZE_X = 1;
    const int World::WORLD_SIZE_Y = 1;

    void World::Init()
    {
        Engine::ResourceLoader::SetScaleMode(SDL_SCALEMODE_PIXELART);
        SDL_SetWindowFullscreen(Engine::Window::GetWindow(), true);

        camera = new Camera
        (
                (WORLD_SIZE_X * Grid::GRID_SIZE * Tile::TILE_SIZE) / 2.0f,
                (WORLD_SIZE_Y * Grid::GRID_SIZE * Tile::TILE_SIZE) / 2.0f,
            1.0f
        );

        camera->minZoom = 0.05;
        camera->limitBounds = true;
        camera->limitLeft = 0.0f;
        camera->limitRight = WORLD_SIZE_X * Grid::GRID_SIZE * Tile::TILE_SIZE;
        camera->limitTop = 0.0f;
        camera->limitBottom = WORLD_SIZE_Y * Grid::GRID_SIZE * Tile::TILE_SIZE;

        for (int y = 0; y < WORLD_SIZE_Y; ++y)
        {
            for (int x = 0; x < WORLD_SIZE_X; ++x)
            {
                Engine::Vec2<int> gridPosition{x, y};
                areas[gridPosition] = new Area(x, y);
            }
        }
    }

    void World::HandleEvents(SDL_Event &event)
    {

    }

    void World::Update(float delta)
    {
        Engine::Vec2<float> camPos = Engine::Camera::main->position;

        Engine::Vec2<int> camAreaPos{
            (int)std::floor(camPos.x / (Grid::GRID_SIZE * Tile::TILE_SIZE)),
            (int)std::floor(camPos.y / (Grid::GRID_SIZE * Tile::TILE_SIZE))
        };

        cameraAreaPosition = camAreaPos;
    }

    void World::Render()
    {
        RenderAreas();

        ImGui::SetNextWindowPos({0,0});
        ImGui::Begin("Debug");

        ImGui::Text("Camera Position : (%.0f, %.0f)", camera->position.x, camera->position.y);
        ImGui::Text("Camera Area Position : (%d, %d)", cameraAreaPosition.x, cameraAreaPosition.y);

        int areaPixelSize = Grid::GRID_SIZE * Tile::TILE_SIZE;

        int areasVisibleX = (int)std::ceil((float)Engine::Window::viewport.x / (areaPixelSize * Engine::Camera::main->zoom)) + 1;
        int areasVisibleY = (int)std::ceil((float)Engine::Window::viewport.y / (areaPixelSize * Engine::Camera::main->zoom)) + 1;

        ImGui::Text("Rendered Area : (%d, %d)", areasVisibleX, areasVisibleY);

        float mX = 0;
        float mY = 0;
        SDL_GetMouseState(&mX, &mY);

        Engine::Vec2<float> mouseLocalPosition{mX, mY};
        Engine::Vec2<float> offset = (Engine::Vec2<float>)Engine::Window::viewport / 2.0f;
        Engine::Vec2<float> mouseGlobalPosition = camera->position + ((mouseLocalPosition - offset) / camera->zoom);

        ImGui::Text("Mouse Position : (%.0f, %.0f)", mouseGlobalPosition.x, mouseGlobalPosition.y);

        ImGui::Text("Viewport Size : (%d, %d)", Engine::Window::viewport.x, Engine::Window::viewport.y);
        ImGui::End();
    }

    void World::RenderAreas()
    {
        int areaPixelSize = Grid::GRID_SIZE * Tile::TILE_SIZE;

        int areasVisibleX = (int)std::ceil((float)Engine::Window::viewport.x / (areaPixelSize * Engine::Camera::main->zoom)) + 1;
        int areasVisibleY = (int)std::ceil((float)Engine::Window::viewport.y / (areaPixelSize * Engine::Camera::main->zoom)) + 1;

        for (int y = -areasVisibleY/2; y <= areasVisibleY/2; ++y)
        {
            for (int x = -areasVisibleX/2; x <= areasVisibleX/2; ++x)
            {
                Engine::Vec2<int> offset{x, y};
                Engine::Vec2<int> currPos = offset + cameraAreaPosition;
                if (areas.contains(currPos))
                {
                    Area *currentArea = areas[currPos];
                    currentArea->RenderTiles();
                }
            }
        }
    }

    void World::RenderEntities()
    {
    }

    void World::Clean()
    {
        entities.clear();
        for (auto &kvp : areas)
        {
            delete kvp.second;
        }
        areas.clear();
        delete camera;
    }
}



//        if (networkManager.clientId < 0)
//        {
//                SDL_SetRenderDrawColor(renderer, 50, 50, 50, SDL_ALPHA_OPAQUE);
//                SDL_RenderClear(renderer);
//                ImGui_ImplSDLRenderer3_NewFrame();
//                ImGui_ImplSDL3_NewFrame();
//                ImGui::NewFrame();
//
//                int w = 0;
//                int h = 0;
//
//                SDL_GetWindowSize(window, &w, &h);
//
//                ImGui::SetNextWindowPos(ImVec2(w/2.0f, h/2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
//                ImGui::Begin("Connect to Server", nullptr,
//                             ImGuiWindowFlags_NoResize |
//                             ImGuiWindowFlags_AlwaysAutoResize |
//                             ImGuiWindowFlags_NoMove |
//                             ImGuiWindowFlags_NoScrollbar |
//                             ImGuiWindowFlags_NoSavedSettings |
//                             ImGuiWindowFlags_NoCollapse);
//
//                ImGui::InputText("Username", networkManager.username, IM_ARRAYSIZE(networkManager.username));
//
//                if (ImGui::Button("Connect") && !networkManager.triedConnect)
//                {
//                    networkManager.triedConnect = true;
//
//                    // Build a packet with username
//                    Engine::ConnectPacket connectPacket{};
//                    connectPacket.type = Engine::PACKET_CONNECT;
//                    connectPacket.clientId = -1;
//                    strncpy(connectPacket.username, networkManager.username, sizeof(connectPacket.username)-1);
//
//                    ENetPacket* packet = enet_packet_create(&connectPacket, sizeof(connectPacket), ENET_PACKET_FLAG_RELIABLE);
//                    enet_peer_send(networkManager.server, 0, packet);
//                }
//
//                ImGui::End();
//
//                ImGui::Render();
//                ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
//                SDL_RenderPresent(renderer);
//
//                continue;
//        }


//        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
//        {
//            if (event.button.button == SDL_BUTTON_LEFT)
//            {
//                float mouseX = event.button.x;
//                float mouseY = event.button.y;
//
//                PositionPacket positionPacket;
//                positionPacket.type = PACKET_POSITION;
//                positionPacket.clientId = networkManager.clientId;
//
//                int winW = 0, winH = 0;
//                SDL_GetWindowSize(window, &winW, &winH);
//
//                positionPacket.x = Camera::main->position.x + mouseX / Camera::main->zoom;
//                positionPacket.y = Camera::main->position.y + mouseY / Camera::main->zoom;
//
//                ENetPacket* packet = enet_packet_create(&positionPacket, sizeof(PositionPacket), ENET_PACKET_FLAG_RELIABLE);
//                enet_peer_send(networkManager.server, 0, packet);
//            }
//        }
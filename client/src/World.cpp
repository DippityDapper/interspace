#include "client/World.hpp"

#include "SDL3/SDL.h"

#include "dapper2d/Camera.hpp"
#include "dapper2d/ResourceLoader.hpp"

#include "client/Area.hpp"

namespace Game
{
    void World::Init()
    {
        Engine::ResourceLoader::SetScaleMode(SDL_SCALEMODE_PIXELART);

        camera = new Engine::Camera(0, 0, 1);
        camera->minZoom = 0.01;

        worldSize.x = 2;
        worldSize.y = 1;

        for (int y = 0; y < worldSize.y; ++y)
        {
            for (int x = 0; x < worldSize.x; ++x)
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

    }

    void World::Render()
    {
//        Area* currentArea = GetCurrentArea();
//        currentArea->RenderTiles();
        for (auto &kvp : areas)
        {
            kvp.second->RenderTiles();
        }
        //RenderEntities();
    }

    void World::RenderEntities()
    {
        for (auto& kvp : entities)
        {
            kvp.second->Render();
        }
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

    Area* World::GetCurrentArea()
    {
        if (!areas.contains(currentAreaPosition))
            return nullptr;
        return areas[currentAreaPosition];
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
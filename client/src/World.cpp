#include "client/World.hpp"

#include "imgui.h"
#include "SDL3/SDL.h"

#include "dapper2d/ResourceLoader.hpp"
#include "dapper2d/Window.hpp"
#include "dapper2d/CFGParser.hpp"

#include "client/Camera.hpp"
#include "client/Area.hpp"
#include "client/Tile.hpp"

namespace Game
{
    uint32_t World::worldSeed = 0;
    int World::WORLD_SIZE_X = 0;
    int World::WORLD_SIZE_Y = 0;

    void World::Init()
    {
        Engine::ResourceLoader::SetScaleMode(SDL_SCALEMODE_PIXELART);
        SDL_SetWindowFullscreen(Engine::Window::GetWindow(), true);

        Engine::CFGParser::LoadConfig("world");

        Tiles::InitRegistry();

        worldSeed = Engine::CFGParser::GetUInt32("world", "world_seed");
        WORLD_SIZE_X = Engine::CFGParser::GetInt("world", "world_size_x");
        WORLD_SIZE_Y = Engine::CFGParser::GetInt("world", "world_size_y");
        Area::AREA_SIZE = Engine::CFGParser::GetInt("world", "area_size");
        Tile::TILE_SIZE = Engine::CFGParser::GetInt("world", "tile_size");

        camera = new Camera
        (
            (WORLD_SIZE_X * Area::AREA_SIZE * Tile::TILE_SIZE) / 2.0f,
            (WORLD_SIZE_Y * Area::AREA_SIZE * Tile::TILE_SIZE) / 2.0f,
            1.0f
        );

        camera->minZoom = 0.1;
        camera->limitBounds = true;
        camera->limitLeft = 0.0f;
        camera->limitRight = WORLD_SIZE_X * Area::AREA_SIZE * Tile::TILE_SIZE;
        camera->limitTop = 0.0f;
        camera->limitBottom = WORLD_SIZE_Y * Area::AREA_SIZE * Tile::TILE_SIZE;

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
        if (event.key.down)
        {
            if (event.key.key == SDLK_E)
            {
                float mX = 0;
                float mY = 0;
                SDL_GetMouseState(&mX, &mY);

                Engine::Vec2<float> mouseLocalPosition{mX, mY};
                Engine::Vec2<float> viewportOffset = (Engine::Vec2<float>)Engine::Window::viewport / 2.0f;

                Engine::Vec2<float> mouseGlobalPosition = camera->position + ((mouseLocalPosition - viewportOffset) / camera->zoom);
                Engine::Vec2<int> mouseTilePosition = (mouseGlobalPosition / Tile::TILE_SIZE).Floor();

                Engine::Vec2<int> mouseAreaPosition = mouseTilePosition / Area::AREA_SIZE;
                Engine::Vec2<int> mouseLocalTilePosition = mouseTilePosition % Area::AREA_SIZE;

                if (areas.contains(mouseAreaPosition))
                {
                    Area* area = areas[mouseAreaPosition];
                    area->UpdateTile(mouseLocalTilePosition, Tiles::STONE_PATH);
                }
            }
        }
    }

    void World::Update(float delta)
    {
        Engine::Vec2<float> cameraPosition = Engine::Camera::main->position;

        cameraTilePosition = (cameraPosition / Tile::TILE_SIZE).Floor();

        float zoom = camera->zoom;

        Engine::Vec2<float> viewportOffset = ((Engine::Vec2<float>)Engine::Window::viewport  / 2.0f) / zoom;

        Engine::Vec2<int> minBounds = ((cameraPosition - viewportOffset) / (Tile::TILE_SIZE * Area::AREA_SIZE)).Floor();
        Engine::Vec2<int> maxBounds = ((cameraPosition + viewportOffset) / (Tile::TILE_SIZE * Area::AREA_SIZE)).Floor();

        for (int y = minBounds.y-1; y <= maxBounds.y+1; ++y)
        {
            for (int x = minBounds.x-1; x <= maxBounds.x+1; ++x)
            {
                Engine::Vec2<int> visibleAreaPosition{x, y};
                if (areas.contains(visibleAreaPosition))
                {
                    areas[visibleAreaPosition]->Update(delta);
                }
            }
        }
    }

    void World::Render()
    {
        RenderAreas();

        ImGui::SetNextWindowPos({0,0});
        ImGui::Begin("Debug");

        Engine::Vec2<int> cameraAreaPosition = cameraTilePosition / Area::AREA_SIZE;
        ImGui::Text("Camera Area Position : (%d, %d)", cameraAreaPosition.x, cameraAreaPosition.y);

        Engine::Vec2<int> cameraLocalTilePosition = cameraTilePosition % Area::AREA_SIZE;
        ImGui::Text("Camera Local Tile Position : (%d, %d)", cameraLocalTilePosition.x, cameraLocalTilePosition.y);

        ImGui::Text("Camera Zoom : %.4f", camera->zoom);

        float mX = 0;
        float mY = 0;
        SDL_GetMouseState(&mX, &mY);

        Engine::Vec2<float> mouseLocalPosition{mX, mY};
        Engine::Vec2<float> viewportOffset = (Engine::Vec2<float>)Engine::Window::viewport / 2.0f;

        Engine::Vec2<float> mouseGlobalPosition = camera->position + ((mouseLocalPosition - viewportOffset) / camera->zoom);
        Engine::Vec2<int> mouseTilePosition = (mouseGlobalPosition / Tile::TILE_SIZE).Floor();

        Engine::Vec2<int> mouseAreaPosition = mouseTilePosition / Area::AREA_SIZE;
        ImGui::Text("Mouse Area Position : (%d, %d)", mouseAreaPosition.x, mouseAreaPosition.y);

        Engine::Vec2<int> mouseLocalTilePosition = mouseTilePosition % Area::AREA_SIZE;
        ImGui::Text("Mouse Local Tile Position : (%d, %d)", mouseLocalTilePosition.x, mouseLocalTilePosition.y);

        Engine::Camera* camera = Engine::Camera::main;
        float zoom = camera->zoom;

        Engine::Vec2<float> cameraPosition = camera->position;
        Engine::Vec2<float> viewportOffset2 = ((Engine::Vec2<float>)Engine::Window::viewport  / 2.0f) / zoom;

        Engine::Vec2<int> minBounds = ((cameraPosition - viewportOffset2) / (Tile::TILE_SIZE * Area::AREA_SIZE)).Floor();
        Engine::Vec2<int> maxBounds = ((cameraPosition + viewportOffset2) / (Tile::TILE_SIZE * Area::AREA_SIZE)).Floor();

        ImGui::Text("Rendered X chunks : (%d, %d)", minBounds.x, maxBounds.x);
        ImGui::Text("Rendered Y chunks : (%d, %d)", minBounds.y, maxBounds.y);

        ImGui::Text("Viewport Size : (%d, %d)", Engine::Window::viewport.x, Engine::Window::viewport.y);
        ImGui::End();
    }

    void World::RenderAreas()
    {
        float zoom = camera->zoom;

        Engine::Vec2<float> cameraPosition = camera->position;
        Engine::Vec2<float> viewportOffset = ((Engine::Vec2<float>)Engine::Window::viewport  / 2.0f) / zoom;

        Engine::Vec2<int> minBounds = ((cameraPosition - viewportOffset) / (Tile::TILE_SIZE * Area::AREA_SIZE)).Floor();
        Engine::Vec2<int> maxBounds = ((cameraPosition + viewportOffset) / (Tile::TILE_SIZE * Area::AREA_SIZE)).Floor();

        for (int y = minBounds.y; y <= maxBounds.y; ++y)
        {
            for (int x = minBounds.x; x <= maxBounds.x; ++x)
            {
                Engine::Vec2<int> visibleAreaPosition{x, y};
                if (areas.contains(visibleAreaPosition))
                {
                    areas[visibleAreaPosition]->Render();
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
